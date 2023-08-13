#pragma once
#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <dwmapi.h>
#include <d3d11.h>
#include <string.h>
#include <memory.h>
#include <fstream>
#include <atlconv.h>
#include <vector>
#include <atlstr.h>
#include <stdlib.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/video.hpp>
#include "FreeImage/FreeImage.h"

#define DIRECTINPUT_VERSION 0x0800
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "ImGui/D3DX11tex.h"
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dx11.lib")
#pragma comment(lib,"Dwmapi.lib")
#pragma warning( disable : 4996 )

#pragma comment(lib,"opencv_core3410d.lib")
#pragma comment(lib,"opencv_imgproc3410d.lib")
#pragma comment(lib,"opencv_imgcodecs3410d.lib")
#pragma comment(lib,"opencv_highgui3410d.lib")
#pragma comment(lib,"opencv_ml3410d.lib")
#pragma comment(lib,"opencv_video3410d.lib")
#pragma comment(lib,"FreeImage.lib")

using namespace cv;
using namespace std;