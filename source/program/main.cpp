#include "lib.hpp"
#include "nn/os.hpp"

nn::os::Tick lastFrameTick = nn::os::Tick(0);
typedef int (*nvnGetPresentInterval_0)(void* nvnWindow);
typedef int (*nvnSyncWait_0)(void* nvnSync, int64_t timeout);
typedef uint64_t (*GetGpuTimeInNS)(void* _this);
void* nvnSyncWait_ptr = 0;
void* nvnWindowGetPresentInterval_ptr = 0;
void* nvnWindowSync = 0;
void* GetGpuTimeInNS_ptr = 0;
bool cutsceneFlag = false;
float frameTime = (1.0/30);
float* windGrassFactorPtr = 0;
int presentInterval = 2;
float deltaMax = (1.0/30);
float FPSavg = (1.0/30);

HOOK_DEFINE_TRAMPOLINE(SyncWait) {

	static void* Callback(uint64_t _this) {
		nvnWindowSync = (void*)(_this + 0xFD0);
		uint64_t flag_struct = *(uint64_t*)exl::util::modules::GetTargetOffset(0xC22328);
		uint8_t UI_flag = 0;
		if (flag_struct) {
			uint64_t struct2 = *(uint64_t*)(flag_struct + 0x3F0);
			if (struct2)
				UI_flag = *(uint8_t*)(struct2+0x168);
		}
		if ((cutsceneFlag || UI_flag == 1) && nvnWindowSync) {
			((nvnSyncWait_0)(nvnSyncWait_ptr))(nvnWindowSync, -1);
		}
		return Orig(_this);
	}
};

HOOK_DEFINE_TRAMPOLINE(EndFramebuffer) {

	static void* Callback(uint64_t _this) {
		
		uint64_t RenderStruct = *(uint64_t*)(_this + 0x5028);

		void* nvnWindow = (void*)(RenderStruct + 0x21A0);

		presentInterval = ((nvnGetPresentInterval_0)(nvnWindowGetPresentInterval_ptr))(nvnWindow);
		int* vSync = (int*)exl::util::modules::GetTargetOffset(0xB74BF0);
		int* vSync_old = (int*)exl::util::modules::GetTargetOffset(0xB74BF4);

		cutsceneFlag = *(bool*)exl::util::modules::GetTargetOffset(0xBD7F40);
		if (!cutsceneFlag) {
			*vSync = presentInterval;
			*vSync_old = presentInterval;
		}
		else {
			*vSync = 2;
			*vSync_old = 2;
			presentInterval = 2;
		}
		
		return Orig(_this);

	}

};


//TODO: Hook 0x7E2B84 and replace values in real time

HOOK_DEFINE_TRAMPOLINE(GetGpuTime) {
	static float Callback(void) {
		void* struct_this = *(void**)exl::util::modules::GetTargetOffset(0xECCEC0);
		uint64_t GPUnanoseconds = ((GetGpuTimeInNS)(GetGpuTimeInNS_ptr))(struct_this);
		float GPUseconds = (float)GPUnanoseconds / 1000000000;
		float lastGPUScale = 0;
		if (!cutsceneFlag && (presentInterval < 2)) {
			FPSavg = ((FPSavg * 9) + frameTime) / 10;
			if (FPSavg > (1.03 * deltaMax)) {
				lastGPUScale = 0;
				return ((FPSavg / deltaMax) * 100.0) + 3.0;
			}
			else if (GPUseconds > (0.86 * deltaMax) && lastGPUScale) {
				return lastGPUScale;
			}
			else {
				lastGPUScale = ((GPUseconds / deltaMax) * 100.0) + 3.0;
				return lastGPUScale;
			}
		}
		else {
			return ((GPUseconds / deltaMax) * 100.0) + 3.0;
		}
	}
};

HOOK_DEFINE_INLINE(GetCpuTime) {
	static void Callback(exl::hook::nx64::InlineCtx* ctx) {
		
		nn::os::Tick tick = nn::os::GetSystemTick();

		if (lastFrameTick != nn::os::Tick(0)) {
			uint64_t frameTimeTemp = nn::os::ConvertToTimeSpan(tick - lastFrameTick).GetNanoSeconds();
			if (frameTimeTemp > 50000000) {
				frameTime = (1.0/20);
			}
			else frameTime = (float)frameTimeTemp / 1000000000;
		}
		lastFrameTick = tick;

		deltaMax = 1.0/30;
		float deltaMin = 1.0/20;
		if (presentInterval == 1) {
			deltaMax = 1.0/60;
		}
		else if (presentInterval == 0) {
			deltaMax = 1.0/120;
		}
		
		float gameSpeed = deltaMax;
		if (!cutsceneFlag) {
			if (frameTime > deltaMin) {
				gameSpeed = deltaMin;
			}
			else if (frameTime > deltaMax) {
				gameSpeed = frameTime;
			}
		}

		uintptr_t gameStruct = *(uintptr_t*)exl::util::modules::GetTargetOffset(0xB8F0D0);
		uintptr_t uiStruct = *(uintptr_t*)exl::util::modules::GetTargetOffset(0xC222C8);
		if (gameStruct) {
			float* gameSpeedPtr1 = (float*)(gameStruct + 0x78);
			float* gameSpeedPtr2 = (float*)(gameStruct + 0x7C);
			*windGrassFactorPtr = gameSpeed;
			*gameSpeedPtr1 = gameSpeed;
			*gameSpeedPtr2 = gameSpeed;
			
		}
		if (uiStruct) {
			float* uiSpeedPtr = (float*)(uiStruct + 0x80);
			*uiSpeedPtr = gameSpeed;
		}
	}
};

extern "C" void exl_main(void* x0, void* x1) {
	/* Setup hooking enviroment. */
	exl::hook::Initialize();

	EndFramebuffer::InstallAtOffset(0x6F480C);
	SyncWait::InstallAtOffset(0x7029F8);
	nvnSyncWait_ptr = (void*)(exl::util::GetSdkModuleInfo().m_Total.m_Start + 0x2C8360);
	nvnWindowGetPresentInterval_ptr = (void*)(exl::util::GetSdkModuleInfo().m_Total.m_Start + 0x2CAB68);

	GetGpuTime::InstallAtOffset(0x6A3084);
	GetCpuTime::InstallAtOffset(0x700164);

	// Wind speed factor from MAIN+0x15EC500
	exl::patch::CodePatcher p(0x747DD8);
	p.WriteInst(0xB0007528);
	p.WriteInst(0xBD450106);

	windGrassFactorPtr = (float*)exl::util::modules::GetTargetOffset(0x15EC500);
	*windGrassFactorPtr = (1.0/30);

	GetGpuTimeInNS_ptr = (void*)exl::util::modules::GetTargetOffset(0x6F489C);
}

extern "C" NORETURN void exl_exception_entry() {
	/* TODO: exception handling */
	EXL_ABORT(0x420);
}