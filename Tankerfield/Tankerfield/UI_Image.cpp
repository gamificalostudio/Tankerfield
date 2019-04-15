#include "UI_Image.h"
#include "M_Render.h"
#include "M_UI.h"
#include "App.h"

UI_Image::UI_Image(fPoint position, UI_ImageDef definition, UI_Listener* listener) : UI_Element(position, definition, listener), image_animation(definition.image_animation)
{

}


UI_Image::~UI_Image()
{

}

