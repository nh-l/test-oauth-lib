#pragma once

#include "UserProfile.h"

#include <windows.h>
#include <gdiplus.h>
#include <urlmon.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "urlmon.lib")

using namespace Gdiplus;

namespace
{
	ULONG_PTR gdiplusToken;
	UserProfile userProfile;
}

namespace PW
{
	void initialize()
	{
		GdiplusStartupInput gpsui;
		GdiplusStartup(&gdiplusToken, &gpsui, NULL);
	}

	void shutDown()
	{
		GdiplusShutdown(gdiplusToken);
	}

	void setUserProfile(const UserProfile& up)
	{
		userProfile = up;
	}

	bool isProfileAvailable()
	{
		return userProfile.email.empty() == false;
	}

	void clearUserProfile()
	{
		userProfile = {};
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static Image* image = nullptr;

	switch (uMsg)
	{
		case WM_CREATE:
		{
			if (PW::isProfileAvailable())
			{
				const auto imageUrl = userProfile.profileImage;
				const auto imageFileName = L"profileImage.jpg";
				HRESULT hr = URLDownloadToFile(NULL, imageUrl.data(), imageFileName, 0, NULL);
				if (SUCCEEDED(hr))
				{
					image = new Image(imageFileName);
					DeleteFile(imageFileName);
				}
			}
		}
		break;

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			Graphics graphics(hdc);
			{
				SolidBrush whiteBrush(Color(255, 255, 255, 255));
				graphics.FillRectangle(&whiteBrush, 0, 0, 500, 500);
			}
			if (PW::isProfileAvailable())
			{
				{
					FontFamily ff(L"Arial");
					Font font(&ff, 20);
					SolidBrush brush(Color(255, 0, 0, 0));
					graphics.DrawString(userProfile.username.data(), -1, &font, PointF(10, 10), &brush);
					graphics.DrawString(userProfile.email.data(), -1, &font, PointF(10, 50), &brush);
				}
				{
					if (image)
					{
						graphics.DrawImage(image, 10, 100, image->GetWidth(), image->GetHeight());
					}
				}
			}
			else
			{
				{
					FontFamily ff(L"Arial");
					Font font(&ff, 20);
					SolidBrush brush(Color(255, 0, 0, 0));
					graphics.DrawString(L"로그인이 필요합니다.", -1, &font, PointF(10, 10), &brush);
				}
			}
			EndPaint(hwnd, &ps);
		}
		break;

		case WM_CLOSE:
		{
			PW::clearUserProfile();
			delete image;
			image = nullptr;

			DestroyWindow(hwnd);
		}
		break;

		case WM_DESTROY:
		{
			PostQuitMessage(0);
		}
		break;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}