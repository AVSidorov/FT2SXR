//=********************************************************
//
// BASESVM.CPP
//
// BRD_SveamCollection Class
//
// (C) Instrumental Systems
//
// Created: Ekkore Oct. 2003
//
//=********************************************************


//=********************************************************
//
// Class BRD_SveamCollection
//
//=********************************************************


//=******** BRD_SveamCollection::BRD_SveamCollection ******
//=********************************************************
class BRD_SveamCollection
{

	//
	// Variables
	//
public:
	U32		m_DescAddress;


	//
	// Methods
	//
public:

	BRD_SveamCollection( const char *devName, const char *rsrcName );
	virtual ~BRD_SveamCollection( );

	PVOID operator new(size_t Size) {return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, Size );};
	VOID operator delete(PVOID pVoid) {if(pVoid) HeapFree( GetProcessHeap(), 0, pVoid ); };
	
	S32		IsValid( void ) { reurn 1; };

	S32		LoadProg( const char *fileName );
};


#endif	//__BASESVM_H_


//
// End of File
//

