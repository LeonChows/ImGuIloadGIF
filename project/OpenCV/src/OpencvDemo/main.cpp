#include "head.h"

#define GIF_LUJING "C:\\Users\\Administrator\\Desktop\\1\\3.gif"
/*
	============================
	=  Yuna_Make q3144994852   =
	= Email 3144994852@qq.com  =
	=        2023/1/3          =
	============================
*/
//dx11驱动
static ID3D11Device* g_pd3dDevice = NULL;
//dx11上下文
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
//dx交换链
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;
static ID3D11ShaderResourceView* m_srv[1000];

static float Q_sum = 0.f;
static bool Q_pause = false;
static int Q_sum2;
static int Q_height;
static int Q_width;
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static int i = 0;
static int Q_count = 0;
static int test;
static char texts[255];
//图片速度
static int run = 50;
static const char* items[] = { u8"低速",u8"中速",u8"高速" };
static int item_current = 1;
class LoadGIF
{
public:

	FREE_IMAGE_FORMAT fif;
	BYTE* bits;
	Mat Src_Gif;
	// ------------ - GIF文件的载入
		/*首先是利用 FreeImage_GetFileType()函数，通过文件名，获取文件的类型；
		 然后判断 FreeImage_FIFSupportsReading()，是否是FreeImage支持的文件类型；
		 之后获取文件的相关信息，位数，大小。
		 再利用 FreeImage_OpenMultiBitmap，以 GIF_DEFAULT 的方式加载Gif文件，关于这个函数参数的意义，可查看我上面给出的文档。
		 利用 FreeImage_GetPageCount 函数获取帧数。
		 之后就是在一个for循环里面，使用 Gif_To_Mat，将每一帧图片转换为Mat。
		 再使用 imwrite 将图像储存到本地。
	*/
	bool Gif_Load(const string & filename)
	{
		FIBITMAP* dib = 0;
		FIMULTIBITMAP* bitmap = 0;
		FIBITMAP* pFrame;

		fif = FreeImage_GetFileType(filename.c_str(), 0);
		if (fif == FIF_UNKNOWN)                     fif = FreeImage_GetFIFFromFilename(filename.c_str());
		if (fif == FIF_UNKNOWN)                     return false;
		if (FreeImage_FIFSupportsReading(fif))      dib = FreeImage_Load(fif, filename.c_str());
		if (!dib)                                   return false;//dib Load failed  

		//bpp = FreeImage_GetBPP(dib);
		bits = (BYTE*)FreeImage_GetBits(dib);
		Q_width = FreeImage_GetWidth(dib);
		Q_height = FreeImage_GetHeight(dib);

		cout << "Load The File:   " << filename.c_str() << endl;
		cout << "The File's width: " << Q_width << endl;
		cout << "The File's height: " << Q_height << endl;

		if ((bits == 0) || (Q_width == 0) || (Q_height == 0))       return false;

		bitmap = FreeImage_OpenMultiBitmap(fif, filename.c_str(), 0, 0, 1, GIF_DEFAULT);
		if (bitmap == NULL)
		{
			cout << "BitMap == Null" << endl;
			return FALSE;
		}

		Q_count = FreeImage_GetPageCount(bitmap);//获取帧数；

		for (int i = 0; i <= Q_count - 1; i++)
		{
			pFrame = FreeImage_LockPage(bitmap, i);
			//cout << "pFrame:" << pFrame << endl;

			Src_Gif = Gif_To_Mat(pFrame, fif);  //转换为Mat；
			string Src_Gif_Name = to_string(i);
			imwrite(Src_Gif_Name + ".png", Src_Gif);

			FreeImage_UnlockPage(bitmap, pFrame, 1);
		}
		cout << "初始化完毕..." << endl;
		FreeImage_Unload(dib);
		FreeImage_DeInitialise();
		Load_flag = TRUE;
		return Load_flag;
	}
	//------- GIF 转换到Mat
	Mat Gif_To_Mat(FIBITMAP* fiBmp, const FREE_IMAGE_FORMAT fif)
	{
		if (fiBmp == NULL || fif != FIF_GIF)
		{
			return Mat();
		}

		BYTE intensity;
		BYTE* PIintensity = &intensity;
		if (FreeImage_GetBPP(fiBmp) != 8)
			fiBmp = FreeImage_ConvertTo8Bits(fiBmp);

		RGBQUAD* pixels = new RGBQUAD;
		pixels = FreeImage_GetPalette(fiBmp);

		Mat img = Mat::zeros(Q_height, Q_width, CV_8UC3);

		uchar* p;

		for (int i = 0; i < Q_height; i++)
		{
			p = img.ptr<uchar>(i);
			for (int j = 0; j < Q_width; j++)
			{
				FreeImage_GetPixelIndex(fiBmp, j, Q_height - i, PIintensity);
				p[3 * j] = pixels[intensity].rgbBlue;
				p[3 * j + 1] = pixels[intensity].rgbGreen;
				p[3 * j + 2] = pixels[intensity].rgbRed;
			}
		}

		return img;
	}
	bool Load_flag;
};

class ImGuiDrawPhoto
{
public:

	//帮助提示符
	void Helpmarker(const char* Text, const ImVec4& Color)
	{
		ImGui::TextColored(Color, u8"(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(Text);
		}
	}
	//循环修改全局值
	static void Pause()
	{
		while (true)
		{
			if (Q_pause != true)
			{
				i += 1;
				if (i == Q_count)
				{
					i = 0;
				}
			}
			Sleep(run);
		}
	}
   /// <summary>
   /// 获取当前执行目录
   /// </summary>
   /// <returns></returns>
    //获取当前输出路径
	string GetProgramDir()
	{
		char exeFullPath[MAX_PATH]; // Full path
		string strPath = "";

		GetModuleFileNameA(NULL, exeFullPath, MAX_PATH);
		strPath = (string)exeFullPath;    // Get full path of the file
		int pos = strPath.find_last_of('\\', strPath.length());
		return strPath.substr(0, pos);  // Return the directory without the file name
	}
	/// <summary>
	/// 
	/// #include <iostream>
	/// #include <fstream>
	/// 函数说明:  读二进制文件
	///参数描述: _fileName, 文件名称
	///	_buf, 读出来的数据存放位置
	/// _bufLen, 数据的长度信息
	/// 返回值 : true, 成功
	/// false, 失败
	/// </summary>
	/// <param name="_fileName"></param>
	/// <param name="_buf"></param>
	/// <param name="_bufLen"></param>
	/// <returns></returns>
	 //读取文件
	bool readFile(const char* _fileName, unique_ptr<char[]>& _buf, int& _bufLen)
	{
		filebuf* pbuf;
		ifstream file;
		// 要读入整个文件，必须采用二进制打开 
		file.open(_fileName, ios::binary);
		// 获取filestr对应buffer对象的指针 
		pbuf = file.rdbuf();
		if (!pbuf)return false;

		// 调用buffer对象方法获取文件大小
		_bufLen = pbuf->pubseekoff(0, ios::end, ios::in);
		if (!_bufLen) return false;
		pbuf->pubseekpos(0, ios::in);

		// 分配内存空间
		_buf.reset(new char[_bufLen] {0});
		// 获取文件内容
		if (pbuf->sgetn(reinterpret_cast<char*>(_buf.get()), _bufLen) != _bufLen)return false;
		file.close();

		return true;
	}
	//从资源载入图片
	ID3D11ShaderResourceView* DX11LoadTextureImageToExeFilePath(ID3D11Device* pD3dDevice, string file_name)
	{
		//读取资源文件
		unique_ptr<char[]> ptr;
		int szie = 0;
		auto file_path = GetProgramDir();
		file_path += "\\";
		file_path += file_name;

		if (readFile(file_path.c_str(), ptr, szie))
		{
			ID3D11Texture2D* pTexture2D = NULL;
			D3D11_TEXTURE2D_DESC dec;

			HRESULT result;
			D3DX11_IMAGE_LOAD_INFO loadInfo;
			ZeroMemory(&loadInfo, sizeof(D3DX11_IMAGE_LOAD_INFO));
			loadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			loadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			loadInfo.MipLevels = D3DX11_DEFAULT; //这时会产生最大的mipmaps层。
			loadInfo.MipFilter = D3DX11_FILTER_LINEAR;

			result = D3DX11CreateTextureFromMemory(
				pD3dDevice,
				ptr.get(),
				szie,
				&loadInfo,
				NULL,
				(ID3D11Resource**)(&pTexture2D),
				NULL
			);

			if (result != S_OK) {
				return NULL;
			}

			pTexture2D->GetDesc(&dec);
			ID3D11ShaderResourceView* pTextureView = NULL;

			// Create texture view
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			ZeroMemory(&srvDesc, sizeof(srvDesc));
			srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MipLevels = dec.MipLevels;
			srvDesc.Texture2D.MostDetailedMip = 0;
			pD3dDevice->CreateShaderResourceView(pTexture2D, &srvDesc, &pTextureView);

			return pTextureView;
		}
		else
		{
			return NULL;
		}
	}
	//主要菜单功能
	void DrawMemu() {

		ImGui::Begin(u8"ImGui Load Gif");
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		ImGui::Checkbox(u8"暂停", &Q_pause);
		ImGui::SliderInt(u8"图片帧率", &i, 0, Q_count - 1);
		const char* combo_preview_value = items[item_current];
		if (ImGui::BeginCombo(u8"图片速度", combo_preview_value))
		{
			for (int n = 0; n < IM_ARRAYSIZE(items); n++)
			{
				const bool is_selected = (item_current == n);
				if (ImGui::Selectable(items[n], is_selected))
					item_current = n;
				if (item_current == 0) {
					run = 300;
				}				
				if (item_current == 1) {
					run = 50;
				}				
				if (item_current == 2) {
					run = 30;
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		ImGui::ImageButton(m_srv[i], ImVec2(64, 32));
	

		//ImGui::ImageButtonEx(1,m_srv[i], ImVec2(32, 16),);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		auto drawList = ImGui::GetOverlayDrawList();
		string File_Name = to_string(i);
		drawList->AddImage(m_srv[i], ImVec2(400, 400), ImVec2(Q_width * 3, Q_height * 3), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 255, 255, 255));
		ImGui::End();
		ImGui::Image(m_srv[i],ImVec2(Q_width, Q_height));
	}
};

class ImGuiClean
{
public:
	//创建D3D设备
	bool CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}
	//清理D3D设备
	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}
	//创建渲染目标
	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
	//清理渲染目标
	void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

};
LoadGIF Go;
ImGuiDrawPhoto Draw;
ImGuiClean Clean;
//主函数
int main(int argc, char** argv)
{

	Go.Gif_Load(GIF_LUJING);
	CreateThread(NULL, NULL,(LPTHREAD_START_ROUTINE)Draw.Pause, NULL, NULL,NULL);
	//开启高dpi感知
	//ImGui_ImplWin32_EnableDpiAwareness();
	//声明一个窗口类
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	//注册这个窗口类
	::RegisterClassEx(&wc);
	//创建这个窗口
	HWND hwnd = ::CreateWindowExW(/*WS_EX_TOPMOST*//*WS_EX_LAYERED*/0, wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	//初始化d3d设备
	if (!Clean.CreateDeviceD3D(hwnd))
	{
		//失败的清理
		Clean.CleanupDeviceD3D();
		//销毁窗口类
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}
	// 显示窗口
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	// 更新窗口 发送更新消息
	::UpdateWindow(hwnd);
	// imgui结构检查
	IMGUI_CHECKVERSION();
	// 初始化imgui
	ImGui::CreateContext();
	//获取imgui
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//设置不生成imgui窗口位置存贮
	io.WantSaveIniSettings = false;
	io.IniFilename = NULL;
	// imgui的三个默认样式
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();
	//初始化cpu频率和当前开机时间以及事件类型
	ImGui_ImplWin32_Init(hwnd);
	//保存dx11到imgui设备和上下文
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	//字体配置
	ImFontConfig f_cfg;
	f_cfg.FontDataOwnedByAtlas = false;
	const ImFont* font =  io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msyh.ttc",18.f, &f_cfg, io.Fonts->GetGlyphRangesChineseFull());
	//IM_ASSERT(font != NULL);
	// Our state
	bool show_demo_window = true;
	bool show_another_window = false;
	//SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	//DWM_BLURBEHIND bb = { 0 };
	//HRGN hRgn = CreateRectRgn(0, 0, -1, -1);
	//bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	//bb.hRgnBlur = hRgn;
	//bb.fEnable = TRUE;
	//DwmEnableBlurBehindWindow(hwnd, &bb);
	/*
		dx刷新颜色
		渲染窗口颜色
	*/
	ImVec4 clear_color = ImColor(100, 98, 88, 255);
	//ImVec4 clear_color = ImGui::ColorConvertU32ToFloat4(IM_COL32(0, 0, 0, 0));
	//数组创建纹理
	for (int photo = 0; photo < Q_count; photo++)
	{
		string photo_Name = to_string(photo);
		m_srv[photo] = Draw.DX11LoadTextureImageToExeFilePath(g_pd3dDevice, photo_Name + ".png");
	}
	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	//开始循环创建绘制
	while (msg.message != WM_QUIT)
	{
		//轮询和处理消息
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		//创建dx11着色器 已经以及字体
		ImGui_ImplDX11_NewFrame();
		//获取当前系统鼠标 键盘 游戏手柄的信息
		ImGui_ImplWin32_NewFrame();
		//全局数据更新
		ImGui::NewFrame();
		Draw.DrawMemu();

		//提交imgui绘制数据
		ImGui::Render();
		//绑定到渲染管线
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		//清空屏幕
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
		//将imgui的绘制数据绘制到dx11
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//呈现
		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}
		// Cleanup
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	Clean.CleanupDeviceD3D();
	::DestroyWindow(hwnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);
	return 0;
}
//创建win32
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//WndProc Win消息函数
LRESULT __stdcall WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			Clean.CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			Clean.CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		return 0;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}