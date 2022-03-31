

#include <vector>
#include "brd.h"

namespace insys
{

	class Service
	{
	public:
		BRD_Handle hSrv;

		Service(BRD_Handle h = 0)
		{
			hSrv = h;
		}
		
		operator BRD_Handle&() { return hSrv; }

		inline S32 ctrl(U32 nodeId, U32 cmd, void *arg)
		{
			return BRD_ctrl(hSrv, nodeId, cmd, arg );
		}		
		
		inline S32 ctrl( U32 cmd, void *arg)
		{
			return BRD_ctrl(hSrv, 0, cmd, arg);
		}
		
		inline S32 release(U32 nodeId = 0)
		{
			return BRD_release(hSrv, nodeId);
		}

	};

	class BRD
	{
	protected:
		
		BRD_Handle hBrd;

		inline BRD(U32 lid , U32 flag = 0, void *ptr = NULL)
		{
			BRD_Info Info;

			if (BRD_errcmp(BRD_getInfo(lid, &Info), BRDerr_SHELL_UNINITIALIZED))
				init();
			hBrd = BRD_open(lid, flag, ptr);

		}

	public : 
		
		operator BRD_Handle&() { return hBrd; }
		
		inline static void displayMode(S32 errLevel)
		{
			return BRD_displayMode(errLevel);
		}
		inline static S32 init( const BRDCHAR *iniFile = NULL, S32 *pNum = NULL )
		{
			return initEx( (iniFile) ? BRDinit_FILE : BRDinit_FILE_KNOWN, iniFile, NULL, pNum );
		}			
		
		inline static S32 initEx( U32 mode, const void *pSrc, const BRDCHAR *logFile, S32 *pNum )
		{
			return BRD_initEx( mode, pSrc, logFile, pNum );
		}

		inline static S32 getInfo(U32 lid, BRD_Info *pInfo)
		{
			return BRD_getInfo(lid, pInfo);
		}
		
		inline static BRD open(U32 lid = 0, U32 flag = 0, void *ptr = NULL)
		{
			return BRD(lid, flag, ptr);
		}

		inline static std::vector<U32> lidList()
		{
			U32 Num = 0;
			
			if (BRD_errcmp(BRD_lidList(NULL, 0, &Num) , BRDerr_SHELL_UNINITIALIZED))
				init();

			std::vector<U32> plist = std::vector<U32>( Num );
			BRD_lidList(plist.data(),Num,NULL);
			return plist; 

		}
		
		inline BRD_Info info()
		{
			BRD_Info Info;

			BRD_getInfoEx(hBrd, NULL, &Info);

			return Info;
		}

		inline BRD()
		{
			this->hBrd = 0;
		}

		inline BRD(BRD_Handle hBrd)
		{
			this->hBrd = hBrd;
		}	
		
		inline ~BRD(  )
		{
			BRD_close( hBrd );
			hBrd = 0;
		}

		inline S32 load( const BRDCHAR *fileName, int argc=0, BRDCHAR *argv[]=NULL )
		{
			return BRD_load( hBrd, 0, fileName, argc, argv );
		}		
		
		inline S32 load( U32 nodeId, const BRDCHAR *fileName, int argc, BRDCHAR *argv[] )
		{
			return BRD_load( hBrd, nodeId, fileName, argc, argv );
		}

		inline std::vector<BRD_PuList>	puList( )
		{
			U32 ItemReal = 0;

			BRD_puList(hBrd, NULL, 0, &ItemReal);
			
			std::vector<BRD_PuList> plist = std::vector<BRD_PuList>(ItemReal);

			BRD_puList(hBrd, plist.data(), ItemReal, &ItemReal );

			return plist;
		}

		inline S32	puState( U32 puId, U32 *state)
		{
			return BRD_puState(hBrd, puId, state);
		}
		
		inline S32		puLoad( U32 puId, const BRDCHAR *fileName, U32 *state)
		{
			return BRD_puLoad(hBrd, puId, fileName, state);
		}

		inline S32	extension( U32 nodeId, U32 cmd, void *arg)
		{
			return BRD_extension(hBrd, nodeId, cmd, arg);
		}

		inline std::vector<BRD_ServList> serviceList( U32 nodeId = 0 )
		{
			U32 Num = 0;

			BRD_serviceList(hBrd, nodeId, NULL, 0, &Num);
			std::vector<BRD_ServList> plist = std::vector<BRD_ServList>(Num);
			BRD_serviceList(hBrd,0,plist.data(), Num, NULL);
			
			return plist;
		}
		
		Service capture( const BRDCHAR *servName, U32 *pMode = NULL, U32 timeout = 10000)
		{
		

			BRD_Handle h = BRD_capture(hBrd,0,pMode, servName,timeout );

			return 	Service(h);
		}		
		
		template <typename T> T capture(U32 nodeId, U32 *pMode, const BRDCHAR *servName, U32 timeout = 10000)
		{


			BRD_Handle h = BRD_capture(hBrd, nodeId, pMode, servName, timeout);

			return 	T(h);
		}

	};

	



}