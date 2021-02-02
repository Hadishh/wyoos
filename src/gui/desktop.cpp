#include <gui/desktop.h>

using namespace myos::common;
using namespace myos::drivers;
using namespace myos::gui;

Desktop::Desktop(int32_t w, int32_t h, uint8_t r, uint8_t g, uint8_t b)
: CompositeWidget(0, 0, 0, w, h, r, g, b), MouseEventHandler()
{ 
    MouseX = w / 2;
    MouseY = h / 2;
}

Desktop::~Desktop() { }

void Desktop::Draw(GraphicsContext* gc)
{
    CompositeWidget::Draw(gc);

    for(int i = 0; i < 4; ++i)
    {    
        gc->PutPixel(MouseX - i, MouseY, 0xff, 0xff, 0xff);
        gc->PutPixel(MouseX + i, MouseY, 0xff, 0xff, 0xff);
        gc->PutPixel(MouseX, MouseY - i, 0xff, 0xff, 0xff);
        gc->PutPixel(MouseX, MouseY + i, 0xff, 0xff, 0xff);
    }
}

void Desktop::OnMouseDown(uint8_t button){
    CompositeWidget::OnMouseDown(MouseX, MouseY, button);
}

void Desktop::OnMouseUp(uint8_t button)
{
    CompositeWidget::OnMouseUp(MouseX, MouseY, button);
}

void Desktop::OnMouseMove(int32_t x, int32_t y)
{
    int32_t newMouseX = MouseX + x;
    int32_t newMouseY = MouseY + y;

    if(newMouseX < 0) newMouseX = 0;
    if(newMouseX >= w) newMouseX = w - 1;

    if(newMouseY < 0) newMouseY = 0;
    if(newMouseY >= h) newMouseY = h - 1;
    CompositeWidget::OnMouseMove(MouseX, MouseY, newMouseX, newMouseY);

    MouseX = newMouseX;
    MouseY = newMouseY;
}