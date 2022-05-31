#include <windows.h>
#include <gdiplus.h>
#include <fstream>
#include <random>

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void draw(HDC hdc);

int WINAPI WinMain(HINSTANCE currentInstance, HINSTANCE previousInstance, PSTR cmdLine, INT cmdCount)
{
	// Initialize GDI+
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	
	LPCSTR CLASS_NAME = "MyWindowClass";
	WNDCLASS wc{};
	wc.hInstance = currentInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WndProc;
	RegisterClass(&wc);
	
	CreateWindow(CLASS_NAME, "Grafy 6.2",
		WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		100, 100,
		1400, 700,
		NULL, NULL, NULL, NULL);
	MSG msg{};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
	return 0;
}

LRESULT CALLBACK WndProc(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	
	switch (message)
	{
		case WM_PAINT:
			hdc = BeginPaint(windowHandle, &ps);
			//draw.. 
			draw(hdc);
			//end of draw
			EndPaint(windowHandle, &ps);
			return 0;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
	}
	return DefWindowProc(windowHandle, message, wParam, lParam);
}

double dist(int path[150][2], int x[], int y[], int len) {
	double d = 0;
	for (int i = 0; i < len; i++) {
		d += std::sqrt((double)std::pow(x[path[i][0]] - x[path[i][1]], 2) + (double)std::pow(y[path[i][0]] - y[path[i][1]], 2));
	}
	return d;
}

double dist(int x1, int y1, int x2, int y2) {
	return std::sqrt((double)std::pow(x1 - x2, 2) + (double)std::pow(y1 - y2, 2));
}

void draw(HDC hdc) {
	Gdiplus::Graphics graphics(hdc);
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1);
	Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));
	std::ifstream in("input_150.dat");
	const int len = 150;
	int X[len], Y[len];
	{
		int  x, y, xLast, yLast;
		int i = 0;
		in >> x >> y;
		X[i] = x;
		Y[i] = y;
		i++;
		x += 100;
		y = 100 - y;
		x *= 3;
		y *= 3;
		xLast = x;
		yLast = y;
		graphics.FillEllipse(&brush, x - 5, y - 5, 10, 10);
		while (!in.eof()) {
			in >> x >> y;
			X[i] = x;
			Y[i] = y;
			i++;
			x += 100;
			y = 100 - y;
			x *= 3;
			y *= 3;
			graphics.FillEllipse(&brush, x - 5, y - 5, 10, 10);
			graphics.DrawLine(&pen, x, y, xLast, yLast);
			xLast = x;
			yLast = y;
		}
	}
	std::ofstream logFile("log.txt");
	int path[len][2];
	for (size_t i = 0; i < len - 1; i++)
	{
		path[i][0] = i;
		path[i][1] = i + 1;
	}
	path[len - 1][0] = len - 1;
	path[len - 1][1] = 0;
	double d = dist(path, X, Y, len);
	logFile <<  d << std::endl;

	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(0, len - 1); // define the range
	
	int MAX_IT = std::pow(10, 4);
	srand((unsigned)time(NULL));
	for (int i = 100; i >= 1; i--) {
		 double T = double(i * i) * 0.001;
		 for (int it = 0; it < MAX_IT; it++) {
			int atmp,  ctmp;
			while (true) {
				atmp = distr(gen);
				ctmp = distr(gen);
				int &a = path[atmp][0];
				int &b = path[atmp][1];
				int &c = path[ctmp][0];
				int &d = path[ctmp][1];
				int countC = 0;
				int countD = 0;
				if(a != c && b != c && a != d && countC < 2 && countD < 2) {
					break;
				}
			}
			int &a = path[atmp][0];
			int &b = path[atmp][1];
			int &c = path[ctmp][0];
			int &d = path[ctmp][1];
			
			double d1 = dist(X[a], Y[a], X[b], Y[b]) + //(a,b)
				dist(X[c], Y[c], X[d], Y[d]); // (c,d)
			double d2 = dist(X[a], Y[a], X[c], Y[c]) + // (a, c)
				dist(X[b], Y[b], X[d], Y[d]); // (b, d)

			if (d2 < d1) {
				int tmp = b;
				b = c;
				c = tmp;
				int cc = c;
				int dd = d;
				while (cc != b) {
					for (int j = 0; j < len; j++) {
						if (path[j][0] == cc && path[j][1] != dd) {
							int tmp = path[j][0];
							path[j][0] = path[j][1];
							path[j][1] = tmp;
							cc = path[j][0];
							dd = path[j][1];
							break;
						}
					}
				}
			}
			else {
				double r = (double)rand() / RAND_MAX;
				if(r < std::exp(-(d2 - d1) / T)) {
					int tmp = b;
					b = c;
					c = tmp;
					int cc = c;
					int dd = d;
					while (cc != b) {
						for (int j = 0; j < len; j++) {
							if (path[j][0] == cc && path[j][1] != dd) {
								int tmp = path[j][0];
								path[j][0] = path[j][1];
								path[j][1] = tmp;
								cc = path[j][0];
								dd = path[j][1];
								break;
							}
						}
					}
				}
			}
		}
	}
	
	for (size_t i = 0; i < len; i++) {
		
		int x1 = X[path[i][0]];
		int y1 = Y[path[i][0]];
		int x2 = X[path[i][1]];
		int y2 = Y[path[i][1]];
		x1 += 100;
		y1 = 100 - y1;
		x1 *= 3;
		y1 *= 3;
		x1 += 650;
		graphics.FillEllipse(&brush, x1 - 5, y1 - 5, 10, 10);
		
		x2 += 100;
		y2 = 100 - y2;
		x2 *= 3;
		y2 *= 3;
		x2 += 650;
		graphics.DrawLine(&pen, x1, y1, x2, y2);
	}
	logFile << dist(path, X, Y, len) << "\n";
	logFile.close();
	in.close();
}