#include "Panel.h"

Panel::Panel(const char* name, bool start_active, const char* icon) : name(name), active(start_active), icon(icon)
{
}

Panel::~Panel()
{
}

bool Panel::IsFocused()
{
	return focused;
}

bool Panel::IsOnHover()
{
	return hover;
}