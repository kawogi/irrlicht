// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "COSOperator.h"

#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
#include <SDL_clipboard.h>
#include <SDL_version.h>
#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
#include "CIrrDeviceLinux.h"
#endif

#include "fast_atof.h"

#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
static const bool sdl_supports_primary_selection = [] {
#if SDL_VERSION_ATLEAST(2, 25, 0)
	SDL_version linked_version;
	SDL_GetVersion(&linked_version);
	return (linked_version.major == 2 && linked_version.minor >= 25)
			|| linked_version.major > 2;
#else
	return false;
#endif
}();
#endif

namespace irr
{

#if defined(_IRR_COMPILE_WITH_X11_DEVICE_)
// constructor  linux
	COSOperator::COSOperator(const core::stringc& osVersion, CIrrDeviceLinux* device)
: OperatingSystem(osVersion), IrrDeviceLinux(device)
{
}
#endif

// constructor
COSOperator::COSOperator(const core::stringc& osVersion) : OperatingSystem(osVersion)
{
	#ifdef _DEBUG
	setDebugName("COSOperator");
	#endif
}


COSOperator::~COSOperator()
{
#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
	SDL_free(ClipboardSelectionText);
	SDL_free(PrimarySelectionText);
#endif
}


//! returns the current operating system version as string.
const core::stringc& COSOperator::getOperatingSystemVersion() const
{
	return OperatingSystem;
}


//! copies text to the clipboard
void COSOperator::copyToClipboard(const c8 *text) const
{
	if (strlen(text)==0)
		return;

#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
	SDL_SetClipboardText(text);

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    if ( IrrDeviceLinux )
        IrrDeviceLinux->copyToClipboard(text);
#endif
}


//! copies text to the primary selection
void COSOperator::copyToPrimarySelection(const c8 *text) const
{
	if (strlen(text)==0)
		return;

#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
#if SDL_VERSION_ATLEAST(2, 25, 0)
	if (sdl_supports_primary_selection)
		SDL_SetPrimarySelectionText(text);
#endif

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    if ( IrrDeviceLinux )
        IrrDeviceLinux->copyToPrimarySelection(text);
#endif
}


//! gets text from the clipboard
const c8* COSOperator::getTextFromClipboard() const
{
#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
	SDL_free(ClipboardSelectionText);
	ClipboardSelectionText = SDL_GetClipboardText();
	return ClipboardSelectionText;

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
    if ( IrrDeviceLinux )
        return IrrDeviceLinux->getTextFromClipboard();
    return 0;

#else

	return 0;
#endif
}


//! gets text from the primary selection
const c8* COSOperator::getTextFromPrimarySelection() const
{
#if defined(_IRR_COMPILE_WITH_SDL_DEVICE_)
#if SDL_VERSION_ATLEAST(2, 25, 0)
	if (sdl_supports_primary_selection) {
		SDL_free(PrimarySelectionText);
		PrimarySelectionText = SDL_GetPrimarySelectionText();
		return PrimarySelectionText;
	}
#endif
	return 0;

#elif defined(_IRR_COMPILE_WITH_X11_DEVICE_)
	if ( IrrDeviceLinux )
		return IrrDeviceLinux->getTextFromPrimarySelection();
	return 0;

#else

	return 0;
#endif
}


bool COSOperator::getSystemMemory(u32* Total, u32* Avail) const
{
#if defined(_IRR_POSIX_API_) && defined(_SC_PHYS_PAGES) && defined(_SC_AVPHYS_PAGES)
        long ps = sysconf(_SC_PAGESIZE);
        long pp = sysconf(_SC_PHYS_PAGES);
        long ap = sysconf(_SC_AVPHYS_PAGES);

	if (ps == -1 || (Total && pp == -1) || (Avail && ap == -1))
		return false;

	if (Total)
		*Total = (u32)((pp>>10)*ps);
	if (Avail)
		*Avail = (u32)((ap>>10)*ps);
	return true;
#else
	// TODO: implement for others
	return false;
#endif
}


} // end namespace

