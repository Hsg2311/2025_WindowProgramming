#define NOMINMAX

#pragma warning(disable : 4146)

#include <Windows.h>
#include <system_error>
#include <string>
#include <random>

using i32t = std::int32_t;
using u32t = std::uint32_t;

inline constexpr u32t boardSize = 20u;

inline constexpr i32t clWidth = 800u;
inline constexpr i32t clHeight = 800u;

inline constexpr u32t slotWidth = clWidth / boardSize;
inline constexpr u32t slotHeight = clHeight / boardSize;

inline constexpr std::size_t wordCnt = 5u;
inline constexpr char empty = ' ';
inline constexpr char player = '#';

std::mt19937 gRng{ std::random_device{}() };
std::uniform_int_distribution<u32t> gDistAlphaPosY(2u, boardSize - 2u);
std::uniform_int_distribution<u32t> gDistAlphaPosX(0u, boardSize - 1u);
std::uniform_int_distribution<u32t> gDistWordBinary(0u, 1u);
std::uniform_int_distribution<u32t> gDistWordIdx(0u, wordCnt - 1u);

char gBoard[boardSize][boardSize];
std::string gWords[wordCnt] = { "HELLO", "WORLD", "WINDOW", "PROGRAMMING", "APPLE" };
u32t gChosenWordIdx = -1u;
bool gActive = false;
u32t gPlayerPosX = boardSize / 2;
u32t gPlayerPosY = boardSize - 1;

HBRUSH gWhiteBrush;
HPEN gBlackPen;
HBRUSH gYelloBrush;
HPEN gYelloPen;
HPEN gRedPen;
HFONT gAlphaFont;

void movePlayer(i32t xWeight, i32t yWeight) {
	if ( gPlayerPosY + yWeight < 0 || gPlayerPosY + yWeight >= boardSize
		|| gPlayerPosX + xWeight < 0 || gPlayerPosX + xWeight >= boardSize
	) {
		return;
	}

	if (gBoard[gPlayerPosY + yWeight][gPlayerPosX + xWeight] == empty) {
		gBoard[gPlayerPosY][gPlayerPosX] = empty;
		gPlayerPosX += xWeight;
		gPlayerPosY += yWeight;
		gBoard[gPlayerPosY][gPlayerPosX] = player;
		return;
	}

	if (gPlayerPosY + 2 * yWeight < 0 || gPlayerPosY + 2 * yWeight >= boardSize
		|| gPlayerPosX + 2 * xWeight < 0 || gPlayerPosX + 2 * xWeight >= boardSize
	) {
		return;
	}

	if (gBoard[gPlayerPosY + 2 * yWeight][gPlayerPosX + 2 * xWeight] == empty) {
		gBoard[gPlayerPosY + 2 * yWeight][gPlayerPosX + 2 * xWeight]
			= gBoard[gPlayerPosY + yWeight][gPlayerPosX + xWeight];
	}
	else {
		gBoard[gPlayerPosY + 2 * yWeight][gPlayerPosX + 2 * xWeight]
			= gBoard[gPlayerPosY + yWeight][gPlayerPosX + xWeight]
			+ gBoard[gPlayerPosY + 2 * yWeight][gPlayerPosX + 2 * xWeight] - 'A' + 1;

		while (gBoard[gPlayerPosY + 2 * yWeight][gPlayerPosX + 2 * xWeight] > 'Z') {
			gBoard[gPlayerPosY + 2 * yWeight][gPlayerPosX + 2 * xWeight] -= 26;
		}
	}

	gBoard[gPlayerPosY][gPlayerPosX] = empty;
	gPlayerPosX += xWeight;
	gPlayerPosY += yWeight;
	gBoard[gPlayerPosY][gPlayerPosX] = player;
}

void processCmd(HWND hWnd, WPARAM wParam) {
	switch (wParam) {
	case 'S': {
		if (gActive) {
			gActive = false;
			break;
		}

		gActive = true;
		gChosenWordIdx = gDistWordIdx(gRng);

		// initialize board
		for (u32t i = 0u; i < boardSize; ++i) {
			for (u32t j = 0u; j < boardSize; ++j) {
				gBoard[i][j] = empty;
			}
		}

		for ( u32t i = boardSize / 2 - gWords[gChosenWordIdx].size() / 2, j = 0u;
			j < gWords[gChosenWordIdx].size(); ++i, ++j
		) {
			if (gDistWordBinary(gRng)) {
				gBoard[1][i] = gWords[gChosenWordIdx][j];
			}
		}

		for (u32t i = 0u; i < 52u; ++i) {
			auto posX = gDistAlphaPosX(gRng);
			auto posY = gDistAlphaPosY(gRng);

			while (gBoard[posY][posX] != empty) {
				posX = gDistAlphaPosX(gRng);
				posY = gDistAlphaPosY(gRng);
			}

			gBoard[posY][posX] = 'A' + i % 26;
		}

		gPlayerPosY = boardSize - 1u;
		gPlayerPosX = boardSize / 2u;

		gBoard[gPlayerPosY][gPlayerPosX] = player;
		break;
	}

	case VK_LEFT:
		if (!gActive) {
			break;
		}
		movePlayer(-1, 0);
		break;

	case VK_RIGHT:
		if (!gActive) {
			break;
		}
		movePlayer(1, 0);
		break;

	case VK_UP:
		if (!gActive) {
			break;
		}
		movePlayer(0, -1);
		break;

	case VK_DOWN:
		if (!gActive) {
			break;
		}
		movePlayer(0, 1);
		break;

	case 'Q':
		PostQuitMessage(0);
		break;
	}

	InvalidateRect(hWnd, nullptr, false);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	PAINTSTRUCT ps;
	HDC hDC, memDC;
	HBITMAP memBitmap;


	switch (msg) {
	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_PAINT: {
		hDC = BeginPaint(hWnd, &ps);

		memDC = CreateCompatibleDC(hDC);
		memBitmap = CreateCompatibleBitmap(hDC, clWidth, clHeight);
		SelectObject(memDC, memBitmap);
		SelectObject(memDC, gWhiteBrush);
		SelectObject(memDC, gBlackPen);
		SelectObject(memDC, gAlphaFont);
		PatBlt(memDC, 0, 0, clWidth, clHeight, PATCOPY);

		// rendering =====

		SetTextAlign(memDC, TA_CENTER);

		if (!gActive) {
			SetTextColor(memDC, RGB(65, 65, 65));
			TextOutA(memDC, clWidth / 2, clHeight / 2, "Press 'S' to start", 18);
		}
		else {
			// draw board
			SetTextColor(memDC, RGB(120, 120, 240));

			for (u32t i = 0u; i < boardSize; ++i) {
				MoveToEx(memDC, i * slotWidth, 0, nullptr);
				LineTo(memDC, i * slotWidth, clHeight);
			}

			for (u32t i = 0u; i < boardSize; ++i) {
				MoveToEx(memDC, 0, i * slotHeight, nullptr);
				LineTo(memDC, clWidth, i * slotHeight);
			}

			// draw alphabets and player

			for (u32t i = 0u; i < boardSize; ++i) {
				for (u32t j = 0u; j < boardSize; ++j) {
					if (gBoard[i][j] == empty) {
						continue;
					}

					TextOutA(memDC, j * slotWidth + slotWidth / 2, i * slotHeight + slotHeight / 8, &gBoard[i][j], 1);
				}
			}

			SelectObject(memDC, gRedPen);

			Rectangle(memDC, gPlayerPosX * slotWidth, gPlayerPosY * slotHeight,
				(gPlayerPosX + 1) * slotWidth, (gPlayerPosY + 1) * slotHeight
			);

			SelectObject(memDC, gYelloBrush);
			SelectObject(memDC, gYelloPen);

			Ellipse( memDC, gPlayerPosX * slotWidth + slotWidth / 12, gPlayerPosY * slotHeight + slotHeight / 12,
				(gPlayerPosX + 1) * slotWidth - slotWidth / 12, (gPlayerPosY + 1) * slotHeight - slotHeight / 12
			);
		}

		// ===============

		BitBlt(hDC, 0, 0, clWidth, clHeight, memDC, 0, 0, SRCCOPY);

		DeleteObject(memBitmap);
		DeleteDC(memDC);

		EndPaint(hWnd, &ps);

		if (!gActive) {
			break;
		}

		// check if word is completed
		for (u32t i = 0u; i < boardSize; ++i) {
			for (u32t j = 0u; j < boardSize; ++j) {
				if (std::search(&gBoard[i][0], &gBoard[i][0] + boardSize,
					gWords[gChosenWordIdx].begin(), gWords[gChosenWordIdx].end()
				) != gBoard[i] + boardSize) {
					MessageBoxA(hWnd, "Word completed!", "Message", MB_OK);
					gActive = false;
					InvalidateRect(hWnd, nullptr, false);
					break;
				}
			}
		}

		char tmpBoard[boardSize][boardSize];

		for (u32t i = 0u; i < boardSize; ++i) {
			for (u32t j = 0u; j < boardSize; ++j) {
				tmpBoard[i][j] = gBoard[j][i];
			}
		}

		for (u32t i = 0u; i < boardSize; ++i) {
			for (u32t j = 0u; j < boardSize; ++j) {
				if (std::search(&tmpBoard[i][0], &tmpBoard[i][0] + boardSize,
					gWords[gChosenWordIdx].begin(), gWords[gChosenWordIdx].end()
				) != tmpBoard[i] + boardSize) {
					MessageBoxA(hWnd, "Word completed!", "Message", MB_OK);
					gActive = false;
					InvalidateRect(hWnd, nullptr, false);
					break;
				}
			}
		}

		break;
	}

	case WM_KEYDOWN: case WM_CHAR:
		processCmd(hWnd, wParam);
		break;
	
	case WM_DESTROY:
		PostQuitMessage(0);
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

	auto wndRect = RECT {
		.left = 0,
		.top = 0,
		.right = clWidth,
		.bottom = clHeight
	};

	AdjustWindowRect(&wndRect, WS_OVERLAPPEDWINDOW, false);

	auto hWnd = CreateWindowW(
		L"WindowClass",
		L"Exercise 2_13",
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

	MSG msg;

	gWhiteBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
	gBlackPen = (HPEN)GetStockObject(BLACK_PEN);
	gYelloBrush = CreateSolidBrush(RGB(255, 200, 50));
	gYelloPen = CreatePen(PS_SOLID, 0, RGB(255, 200, 50));
	gRedPen = CreatePen(PS_SOLID, 3, RGB(255, 60, 60));
	gAlphaFont = CreateFontA(32, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial"
	);

	while (GetMessageW(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	DeleteObject(gYelloBrush);
	DeleteObject(gYelloPen);
	DeleteObject(gRedPen);

	return static_cast<int>(msg.wParam);
}