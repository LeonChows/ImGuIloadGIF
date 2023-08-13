#include "head.h"

#define GIF_LUJING "C:\\Users\\Administrator\\Desktop\\1\\3.gif"
/*
	============================
	=  Yuna_Make q3144994852   =
	= Email 3144994852@qq.com  =
	=        2023/1/3          =
	============================
*/
//dx11����
static ID3D11Device* g_pd3dDevice = NULL;
//dx11������
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
//dx������
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
//ͼƬ�ٶ�
static int run = 50;
static const char* items[] = { u8"����",u8"����",u8"����" };
static int item_current = 1;
class LoadGIF
{
public:

	FREE_IMAGE_FORMAT fif;
	BYTE* bits;
	Mat Src_Gif;
	// ------------ - GIF�ļ�������
		/*���������� FreeImage_GetFileType()������ͨ���ļ�������ȡ�ļ������ͣ�
		 Ȼ���ж� FreeImage_FIFSupportsReading()���Ƿ���FreeImage֧�ֵ��ļ����ͣ�
		 ֮���ȡ�ļ��������Ϣ��λ������С��
		 ������ FreeImage_OpenMultiBitmap���� GIF_DEFAULT �ķ�ʽ����Gif�ļ�����������������������壬�ɲ鿴������������ĵ���
		 ���� FreeImage_GetPageCount ������ȡ֡����
		 ֮�������һ��forѭ�����棬ʹ�� Gif_To_Mat����ÿһ֡ͼƬת��ΪMat��
		 ��ʹ�� imwrite ��ͼ�񴢴浽���ء�
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

		Q_count = FreeImage_GetPageCount(bitmap);//��ȡ֡����

		for (int i = 0; i <= Q_count - 1; i++)
		{
			pFrame = FreeImage_LockPage(bitmap, i);
			//cout << "pFrame:" << pFrame << endl;

			Src_Gif = Gif_To_Mat(pFrame, fif);  //ת��ΪMat��
			string Src_Gif_Name = to_string(i);
			imwrite(Src_Gif_Name + ".png", Src_Gif);

			FreeImage_UnlockPage(bitmap, pFrame, 1);
		}
		cout << "��ʼ�����..." << endl;
		FreeImage_Unload(dib);
		FreeImage_DeInitialise();
		Load_flag = TRUE;
		return Load_flag;
	}
	//------- GIF ת����Mat
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

	//������ʾ��
	void Helpmarker(const char* Text, const ImVec4& Color)
	{
		ImGui::TextColored(Color, u8"(?)");
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip(Text);
		}
	}
	//ѭ���޸�ȫ��ֵ
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
   /// ��ȡ��ǰִ��Ŀ¼
   /// </summary>
   /// <returns></returns>
    //��ȡ��ǰ���·��
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
	/// ����˵��:  ���������ļ�
	///��������: _fileName, �ļ�����
	///	_buf, �����������ݴ��λ��
	/// _bufLen, ���ݵĳ�����Ϣ
	/// ����ֵ : true, �ɹ�
	/// false, ʧ��
	/// </summary>
	/// <param name="_fileName"></param>
	/// <param name="_buf"></param>
	/// <param name="_bufLen"></param>
	/// <returns></returns>
	 //��ȡ�ļ�
	bool readFile(const char* _fileName, unique_ptr<char[]>& _buf, int& _bufLen)
	{
		filebuf* pbuf;
		ifstream file;
		// Ҫ���������ļ���������ö����ƴ� 
		file.open(_fileName, ios::binary);
		// ��ȡfilestr��Ӧbuffer�����ָ�� 
		pbuf = file.rdbuf();
		if (!pbuf)return false;

		// ����buffer���󷽷���ȡ�ļ���С
		_bufLen = pbuf->pubseekoff(0, ios::end, ios::in);
		if (!_bufLen) return false;
		pbuf->pubseekpos(0, ios::in);

		// �����ڴ�ռ�
		_buf.reset(new char[_bufLen] {0});
		// ��ȡ�ļ�����
		if (pbuf->sgetn(reinterpret_cast<char*>(_buf.get()), _bufLen) != _bufLen)return false;
		file.close();

		return true;
	}
	//����Դ����ͼƬ
	ID3D11ShaderResourceView* DX11LoadTextureImageToExeFilePath(ID3D11Device* pD3dDevice, string file_name)
	{
		//��ȡ��Դ�ļ�
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
			loadInfo.MipLevels = D3DX11_DEFAULT; //��ʱ���������mipmaps�㡣
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
	//��Ҫ�˵�����
	void DrawMemu() {

		ImGui::Begin(u8"ImGui Load Gif");
		ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
		ImGui::Checkbox(u8"��ͣ", &Q_pause);
		ImGui::SliderInt(u8"ͼƬ֡��", &i, 0, Q_count - 1);
		const char* combo_preview_value = items[item_current];
		if (ImGui::BeginCombo(u8"ͼƬ�ٶ�", combo_preview_value))
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
	//����D3D�豸
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
	//����D3D�豸
	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}
	//������ȾĿ��
	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}
	//������ȾĿ��
	void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

};
LoadGIF Go;
ImGuiDrawPhoto Draw;
ImGuiClean Clean;
//������
int main(int argc, char** argv)
{

	Go.Gif_Load(GIF_LUJING);
	CreateThread(NULL, NULL,(LPTHREAD_START_ROUTINE)Draw.Pause, NULL, NULL,NULL);
	//������dpi��֪
	//ImGui_ImplWin32_EnableDpiAwareness();
	//����һ��������
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	//ע�����������
	::RegisterClassEx(&wc);
	//�����������
	HWND hwnd = ::CreateWindowExW(/*WS_EX_TOPMOST*//*WS_EX_LAYERED*/0, wc.lpszClassName, _T("Dear ImGui DirectX11 Example"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
	//��ʼ��d3d�豸
	if (!Clean.CreateDeviceD3D(hwnd))
	{
		//ʧ�ܵ�����
		Clean.CleanupDeviceD3D();
		//���ٴ�����
		::UnregisterClass(wc.lpszClassName, wc.hInstance);
		return 1;
	}
	// ��ʾ����
	::ShowWindow(hwnd, SW_SHOWDEFAULT);
	// ���´��� ���͸�����Ϣ
	::UpdateWindow(hwnd);
	// imgui�ṹ���
	IMGUI_CHECKVERSION();
	// ��ʼ��imgui
	ImGui::CreateContext();
	//��ȡimgui
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	//���ò�����imgui����λ�ô���
	io.WantSaveIniSettings = false;
	io.IniFilename = NULL;
	// imgui������Ĭ����ʽ
    //ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();
	ImGui::StyleColorsLight();
	//��ʼ��cpuƵ�ʺ͵�ǰ����ʱ���Լ��¼�����
	ImGui_ImplWin32_Init(hwnd);
	//����dx11��imgui�豸��������
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
	//��������
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
		dxˢ����ɫ
		��Ⱦ������ɫ
	*/
	ImVec4 clear_color = ImColor(100, 98, 88, 255);
	//ImVec4 clear_color = ImGui::ColorConvertU32ToFloat4(IM_COL32(0, 0, 0, 0));
	//���鴴������
	for (int photo = 0; photo < Q_count; photo++)
	{
		string photo_Name = to_string(photo);
		m_srv[photo] = Draw.DX11LoadTextureImageToExeFilePath(g_pd3dDevice, photo_Name + ".png");
	}
	// Main loop
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	//��ʼѭ����������
	while (msg.message != WM_QUIT)
	{
		//��ѯ�ʹ�����Ϣ
		if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			continue;
		}
		//����dx11��ɫ�� �Ѿ��Լ�����
		ImGui_ImplDX11_NewFrame();
		//��ȡ��ǰϵͳ��� ���� ��Ϸ�ֱ�����Ϣ
		ImGui_ImplWin32_NewFrame();
		//ȫ�����ݸ���
		ImGui::NewFrame();
		Draw.DrawMemu();

		//�ύimgui��������
		ImGui::Render();
		//�󶨵���Ⱦ����
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		//�����Ļ
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
		//��imgui�Ļ������ݻ��Ƶ�dx11
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
		//����
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
//����win32
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
//WndProc Win��Ϣ����
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