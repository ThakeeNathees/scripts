#ifdef _MSC_VER
	#pragma comment(lib, "opengl32.lib")
	#pragma comment(lib, "user32.lib")
	#pragma comment(lib, "gdi32.lib")
	#ifdef _DEBUG
		#pragma comment(lib, "sfml-main-d.lib")
		#pragma comment(lib, "sfml-audio-d.lib")
		#pragma comment(lib, "sfml-graphics-d.lib")
		#pragma comment(lib, "sfml-network-d.lib")
		#pragma comment(lib, "sfml-window-d.lib")
		#pragma comment(lib, "sfml-system-d.lib")
	#else
		#pragma comment(lib, "sfml-main.lib")
		#pragma comment(lib, "sfml-audio.lib")
		#pragma comment(lib, "sfml-graphics.lib")
		#pragma comment(lib, "sfml-network.lib")
		#pragma comment(lib, "sfml-window.lib")
		#pragma comment(lib, "sfml-system.lib")
	#endif 
#endif
