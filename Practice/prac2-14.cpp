#define NOMINMAX

#pragma warning(disable : 4146)

#include <Windows.h>
#include <system_error>
#include <ranges>
#include <algorithm>
#include <random>
#include <cstdint>

using i32t = std::int32_t;
using u32t = std::uint32_t;

inline constexpr u32t boardSize = 40u;
inline constexpr u32t maxShapeCnt = 10u;

inline constexpr i32t clWidth = 800u;
inline constexpr i32t clHeight = 800u;

inline constexpr u32t slotWidth = clWidth / boardSize;
inline constexpr u32t slotHeight = clHeight / boardSize;

template <class Enum>
constexpr auto etoi(Enum e) noexcept {
	return static_cast<std::underlying_type_t<Enum>>(e);
}

enum class ShapeType {
	None,
	Circle,
	Triangle,
	Square,
};

std::mt19937 gRng{ std::random_device{}() };
std::uniform_int_distribution<u32t> gDistColor(0u, 255u);
std::uniform_int_distribution<u32t> gDistColorIdx(0u, 3u);
std::uniform_int_distribution<u32t> gDistPos(0u, boardSize - 1u);
COLORREF gColors[] = { RGB(0, 162, 232), RGB(126, 132, 247), RGB(161, 250, 79), RGB(240, 135, 132) };
HBRUSH gBrushes[maxShapeCnt];
HPEN gPens[maxShapeCnt];
COLORREF gSetColor;
HBRUSH gSetBrush;
HPEN gSetPen;
ShapeType gSetType = ShapeType::None;
HBRUSH gWhiteBrush;
HPEN gBlackPen;
HPEN gYellowPen;

struct Pos {
	u32t x, y;
};

struct Shape {
	COLORREF color;
	ShapeType type;
	u32t scale;
	Pos pos;
};

Shape gShapes[maxShapeCnt];

u32t gShapeCnt = 0u;
u32t gChosenShapeIdx = -1u;

Pos randomPos() {
	auto x = gDistPos(gRng);
	auto y = gDistPos(gRng);

	while ( std::ranges::any_of(gShapes, [x, y](const auto& shape) {
		return shape.pos.x == x && shape.pos.y == y;
	} ) ) {
		x = gDistPos(gRng);
		y = gDistPos(gRng);
	}

	return Pos{ x, y };
}

void processCmd(HWND hWnd, WPARAM cmd) {
	switch (cmd) {
	case 'E': case 'R': case 'T':
		if (gShapeCnt == maxShapeCnt) {
			DeleteObject(gBrushes[0]);
			DeleteObject(gPens[0]);
			std::move(&gShapes[0 + 1], std::end(gShapes), &gShapes[0]);
			std::move(&gBrushes[0 + 1], std::end(gBrushes), &gBrushes[0]);
			std::move(&gPens[0 + 1], std::end(gPens), &gPens[0]);
			--gShapeCnt;
		}

		gShapes[gShapeCnt] = Shape{
			.color = gColors[gDistColorIdx(gRng)],
			.scale = 1u,
			.pos = randomPos()
		};
		gBrushes[gShapeCnt] = CreateSolidBrush(gShapes[gShapeCnt].color);
		gPens[gShapeCnt] = CreatePen(PS_SOLID, 0, gShapes[gShapeCnt].color);

		switch (cmd) {
		case 'E': gShapes[gShapeCnt].type = ShapeType::Circle; break;
		case 'R': gShapes[gShapeCnt].type = ShapeType::Square; break;
		case 'T': gShapes[gShapeCnt].type = ShapeType::Triangle; break;
		}

		++gShapeCnt;

		break;

	case 'Q':
		PostQuitMessage(0);
		break;

	case VK_LEFT:
		if (gChosenShapeIdx == -1u) {
			break;
		}
		gShapes[gChosenShapeIdx].pos.x -= 1u;
		if (gShapes[gChosenShapeIdx].pos.x == -1u) {
			gShapes[gChosenShapeIdx].pos.x = boardSize - 1u;
		}
		break;

	case VK_RIGHT:
		if (gChosenShapeIdx == -1u) {
			break;
		}
		gShapes[gChosenShapeIdx].pos.x += 1u;
		if (gShapes[gChosenShapeIdx].pos.x == boardSize) {
			gShapes[gChosenShapeIdx].pos.x = 0u;
		}
		break;

	case VK_UP:
		if (gChosenShapeIdx == -1u) {
			break;
		}
		gShapes[gChosenShapeIdx].pos.y -= 1u;
		if (gShapes[gChosenShapeIdx].pos.y == -1u) {
			gShapes[gChosenShapeIdx].pos.y = boardSize - 1u;
		}
		break;

	case VK_DOWN:
		if (gChosenShapeIdx == -1u) {
			break;
		}
		gShapes[gChosenShapeIdx].pos.y += 1u;
		if (gShapes[gChosenShapeIdx].pos.y == boardSize) {
			gShapes[gChosenShapeIdx].pos.y = 0u;
		}
		break;

	case '+':
		if (gChosenShapeIdx == -1u) {
			break;
		}
		gShapes[gChosenShapeIdx].scale = std::min(5u, gShapes[gChosenShapeIdx].scale + 1u);
		break;

	case '-':
		if (gChosenShapeIdx == -1u) {
			break;
		}
		gShapes[gChosenShapeIdx].scale = std::max(1u, gShapes[gChosenShapeIdx].scale - 1u);
		break;

	case 'D':
		if (gChosenShapeIdx == -1u) {
			break;
		}
		DeleteObject(gBrushes[gChosenShapeIdx]);
		DeleteObject(gPens[gChosenShapeIdx]);
		std::move(&gShapes[gChosenShapeIdx + 1], std::end(gShapes), &gShapes[gChosenShapeIdx]);
		std::move(&gBrushes[gChosenShapeIdx + 1], std::end(gBrushes), &gBrushes[gChosenShapeIdx]);
		std::move(&gPens[gChosenShapeIdx + 1], std::end(gPens), &gPens[gChosenShapeIdx]);
		gChosenShapeIdx = std::min(0u, gChosenShapeIdx - 1u);
		--gShapeCnt;
		if (gShapeCnt == 0u) {
			gChosenShapeIdx = -1u;
		}
		break;

	case 'C':
		if (gSetType != ShapeType::None) {
			DeleteObject(gSetBrush);
			DeleteObject(gSetPen);
			gSetType = ShapeType::None;
			break;
		}

		if (gChosenShapeIdx == -1u) {
			break;
		}
		gSetColor = RGB(gDistColor(gRng), gDistColor(gRng), gDistColor(gRng));
		gSetBrush = CreateSolidBrush(gSetColor);
		gSetPen = CreatePen(PS_SOLID, 0u, gSetColor);
		gSetType = gShapes[gChosenShapeIdx].type;
		break;

	case 'P':
		std::ranges::for_each(gBrushes, DeleteObject);
		std::ranges::for_each(gPens, DeleteObject);
		gChosenShapeIdx = -1u;
		gShapeCnt = 0u;
		if (gSetType != ShapeType::None) {
			DeleteObject(gSetBrush);
			DeleteObject(gSetPen);
			gSetType = ShapeType::None;
		}
		break;

	default:
		break;
	}

	if (cmd == '0') {
		if (gShapeCnt == 10u) {
			gChosenShapeIdx = 9u;
		}
		else {
			gChosenShapeIdx = -1u;
		}
	}
	else if (cmd >= '1' && cmd <= '9') {
		if (cmd - '1' < gShapeCnt) {
			gChosenShapeIdx = static_cast<u32t>(cmd - '1');
		}
		else {
			gChosenShapeIdx = -1u;
		}
	}

	InvalidateRect(hWnd, nullptr, false);
}

void Triangle(HDC memDC, int left, int top, int right, int bottom) {
	POINT points[3] = {
		{ (left + right) / 2, top },
		{ left, bottom },
		{ right, bottom }
	};

	Polygon(memDC, points, 3);
}

void Pentagon(HDC memDC, int left, int top, int right, int bottom) {
	POINT points[5] = {
		{ (left + right) / 2, top },
		{ left, (top + bottom) / 2 },
		{ (left * 2 + right) / 3, bottom },
		{ (left + right * 2) / 3, bottom },
		{ right, (top  + bottom) / 2 }
	};

	Polygon(memDC, points, 5);
}

void drawShape(HDC memDC, u32t i) {
	auto& shape = gShapes[i];

	auto color = shape.color;
	auto type = shape.type;
	auto scale = shape.scale;
	auto pos = shape.pos;

	int left = pos.x * slotWidth - slotWidth * (scale - 1u);
	int top = pos.y * slotHeight - slotHeight * (scale - 1u);
	int right = (pos.x + 1) * slotWidth + slotWidth * (scale - 1u);
	int bottom = (pos.y + 1) * slotHeight + slotHeight * (scale - 1u);

	if (type == gSetType) {
		Pentagon(memDC, left, top, right, bottom);
		Pentagon(memDC, left, top - clHeight, right, bottom - clHeight);
		Pentagon(memDC, left, top + clHeight, right, bottom + clHeight);
		Pentagon(memDC, left - clWidth, top, right - clWidth, bottom);
		Pentagon(memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight);
		Pentagon(memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight);
		Pentagon(memDC, left + clWidth, top, right + clWidth, bottom);
		Pentagon(memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight);
		Pentagon(memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight);
		return;
	}

	switch (type) {
	case ShapeType::Circle:
		Ellipse(memDC, left, top, right, bottom);
		Ellipse(memDC, left, top - clHeight, right, bottom - clHeight);
		Ellipse(memDC, left, top + clHeight, right, bottom + clHeight);
		Ellipse(memDC, left - clWidth, top, right - clWidth, bottom);
		Ellipse(memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight);
		Ellipse(memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight);
		Ellipse(memDC, left + clWidth, top, right + clWidth, bottom);
		Ellipse(memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight);
		Ellipse(memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight);
		break;

	case ShapeType::Square:
		Rectangle(memDC, left, top, right, bottom);
		Rectangle(memDC, left, top - clHeight, right, bottom - clHeight);
		Rectangle(memDC, left, top + clHeight, right, bottom + clHeight);
		Rectangle(memDC, left - clWidth, top, right - clWidth, bottom);
		Rectangle(memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight);
		Rectangle(memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight);
		Rectangle(memDC, left + clWidth, top, right + clWidth, bottom);
		Rectangle(memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight);
		Rectangle(memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight);
		break;

	case ShapeType::Triangle:
		Triangle(memDC, left, top, right, bottom);
		Triangle(memDC, left, top - clHeight, right, bottom - clHeight);
		Triangle(memDC, left, top + clHeight, right, bottom + clHeight);
		Triangle(memDC, left - clWidth, top, right - clWidth, bottom);
		Triangle(memDC, left - clWidth, top - clHeight, right - clWidth, bottom - clHeight);
		Triangle(memDC, left - clWidth, top + clHeight, right - clWidth, bottom + clHeight);
		Triangle(memDC, left + clWidth, top, right + clWidth, bottom);
		Triangle(memDC, left + clWidth, top - clHeight, right + clWidth, bottom - clHeight);
		Triangle(memDC, left + clWidth, top + clHeight, right + clWidth, bottom + clHeight);
		break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC, memDC;
	HBITMAP memBitmap;

	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;
	
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN: case WM_CHAR:
		processCmd(hWnd, wParam);
		break;

	case WM_PAINT:
		hDC = BeginPaint(hWnd, &ps);

		memDC = CreateCompatibleDC(hDC);
		memBitmap = CreateCompatibleBitmap(hDC, clWidth, clHeight);
		SelectObject(memDC, memBitmap);
		SelectObject(memDC, gWhiteBrush);
		SelectObject(memDC, gBlackPen);
		PatBlt(memDC, 0, 0, clWidth, clHeight, PATCOPY);

		// rendering =====

		// draw board

		for (u32t i = 0u; i < boardSize; ++i) {
			MoveToEx(memDC, i * slotWidth, 0, nullptr);
			LineTo(memDC, i * slotWidth, clHeight);
		}

		for (u32t i = 0u; i < boardSize; ++i) {
			MoveToEx(memDC, 0, i * slotHeight, nullptr);
			LineTo(memDC, clWidth, i * slotHeight);
		}

		// draw shapes

		for (u32t i = 0u; i < gShapeCnt; ++i) {
			if (i == gChosenShapeIdx) {
				continue;
			}

			if (gShapes[i].type == gSetType) {
				SelectObject(memDC, gSetBrush);
				SelectObject(memDC, gSetPen);
			}
			else {
				SelectObject(memDC, gBrushes[i]);
				SelectObject(memDC, gPens[i]);
			}
			drawShape(memDC, i);
		}

		if (gChosenShapeIdx != -1u) {
			if (gShapes[gChosenShapeIdx].type == gSetType) {
				SelectObject(memDC, gSetBrush);
			}
			else {
				SelectObject(memDC, gBrushes[gChosenShapeIdx]);
			}
			SelectObject(memDC, gYellowPen);
			drawShape(memDC, gChosenShapeIdx);
		}

		// ===============

		BitBlt(hDC, 0, 0, clWidth, clHeight, memDC, 0, 0, SRCCOPY);

		DeleteObject(memBitmap);
		DeleteDC(memDC);

		EndPaint(hWnd, &ps);
		return 0;

	default:
		break;
	}

	return DefWindowProcW(hWnd, msg, wParam, lParam);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
	auto wndCls = WNDCLASSEXW {
		.cbSize = sizeof(WNDCLASSEXW),
		.style = CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = WndProc,
		.cbClsExtra = 0,
		.cbWndExtra = 0,
		.hInstance = hInstance,
		.hIcon = LoadIconW(nullptr, IDI_APPLICATION),
		.hCursor = LoadCursorW(nullptr, IDC_ARROW),
		.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH),
		.lpszMenuName = nullptr,
		.lpszClassName = L"WindowClass",
		.hIconSm = LoadIconW(nullptr, IDI_APPLICATION)
	};

	if (!RegisterClassExW(&wndCls)) {
		OutputDebugStringA(std::system_category().message(GetLastError()).c_str());
		return -1;
	}

	auto wndRect = RECT{ 0, 0, clWidth, clHeight };
	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, false);

	auto hWnd = CreateWindowW(
		L"WindowClass",
		L"Exercise 2_14",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		wndRect.right - wndRect.left,
		wndRect.bottom - wndRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hWnd) {
		OutputDebugStringA(std::system_category().message(GetLastError()).c_str());
		return -1;
	}

	ShowWindow(hWnd, nShowCmd);

	gWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	gBlackPen = (HPEN)GetStockObject(BLACK_PEN);
	gYellowPen = CreatePen(PS_SOLID, 5, RGB(200, 200, 12));

	MSG msg;

	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	DeleteObject(gYellowPen);

	return static_cast<int>(msg.wParam);
}