#ifndef HW_COMPATIBILITY
#define HW_COMPATIBILITY

//---------------------------------------------------------------------------
// FIRMWARE COMPATIBILITIES DEPENDING ON HARDWARE VERSION
//---------------------------------------------------------------------------
// jerome dumas copyright Wave Idea 2004
// jdumas@waveidea.com
//---------------------------------------------------------------------------


const wxString hw_compatiblility[] =
{
	// "HW", connected hardware version, list of compatible hwardware version
	wxT("HW"), wxT("10"), 			// HW 1.0 is only compatible with itself
	wxT("HW"), wxT("11"), wxT("12"), wxT("20"),	// HW 1.1 is compatible with 1.2
	wxT("HW"), wxT("12"), wxT("11"), wxT("20"),	// HW 1.2 is compatible with 1.1
	wxT("HW"), wxT("20"), wxT("12"), wxT("11"),	// HW 2.0 is compatible with 1.2, 1.1
	wxT("END")
};

#endif
