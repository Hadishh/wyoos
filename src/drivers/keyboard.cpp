#include <drivers/keyboard.h>
#include "drivers/kbd.h"
#include <hardwarecommunication/interrupts.h>
using namespace myos::common;
using namespace myos::drivers;
using namespace myos::hardwarecommunication;

void printf(char*);
void printfHex(uint8_t);


KeyboardEventHandler:: KeyboardEventHandler() { }

KeyboardEventHandler::~KeyboardEventHandler() { }

void KeyboardEventHandler:: OnKeyDown(char) { }

void KeyboardEventHandler::OnKeyUp(char) { }


KeyboardDriver:: KeyboardDriver(InterruptManager* manager, KeyboardEventHandler* handler)
: InterruptHandler(0x21, manager),
dataPort(0x60),
commandPort(0x64) { 
    this->handler = handler;
}

KeyboardDriver:: ~KeyboardDriver() { }


void KeyboardDriver:: Activate(){

    while(commandPort.Read() & 0x01)
        dataPort.Read();
    commandPort.Write(0xae); // activate
    commandPort.Write(0x20); // give us your current state
    uint8_t state = (dataPort.Read() | 1) & ~0x10;
    commandPort.Write(0x60); // set state
    dataPort.Write(state);
    dataPort.Write(0xf4); // activate keyboard
    printf("[OK] Keyboard Attached.\n");
}
uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){
    uint8_t key = dataPort.Read();
    if(handler == 0){
        printf("[Warning] There is no handler attached to the keyboard driver, ignoring input.\n");
        return esp;
    }
    static bool shift = false;
    switch (key)
    {
        case 0xfa:
            break;
        case A_KEYCODE: if(shift) handler->OnKeyDown('A'); else handler->OnKeyDown('a'); break;
        case B_KEYCODE: if(shift) handler->OnKeyDown('B'); else handler->OnKeyDown('b'); break;
        case C_KEYCODE: if(shift) handler->OnKeyDown('C'); else handler->OnKeyDown('c'); break;
        case D_KEYCODE: if(shift) handler->OnKeyDown('D'); else handler->OnKeyDown('d'); break;
        case E_KEYCODE: if(shift) handler->OnKeyDown('E'); else handler->OnKeyDown('e'); break;
        case F_KEYCODE: if(shift) handler->OnKeyDown('F'); else handler->OnKeyDown('f'); break;
        case G_KEYCODE: if(shift) handler->OnKeyDown('G'); else handler->OnKeyDown('g'); break;
        case H_KEYCODE: if(shift) handler->OnKeyDown('H'); else handler->OnKeyDown('h'); break;
        case I_KEYCODE: if(shift) handler->OnKeyDown('I'); else handler->OnKeyDown('i'); break;
        case J_KEYCODE: if(shift) handler->OnKeyDown('J'); else handler->OnKeyDown('j'); break;
        case K_KEYCODE: if(shift) handler->OnKeyDown('K'); else handler->OnKeyDown('k'); break;
        case L_KEYCODE: if(shift) handler->OnKeyDown('L'); else handler->OnKeyDown('l'); break;
        case M_KEYCODE: if(shift) handler->OnKeyDown('M'); else handler->OnKeyDown('m'); break;
        case N_KEYCODE: if(shift) handler->OnKeyDown('N'); else handler->OnKeyDown('n'); break;
        case O_KEYCODE: if(shift) handler->OnKeyDown('O'); else handler->OnKeyDown('o'); break;
        case P_KEYCODE: if(shift) handler->OnKeyDown('P'); else handler->OnKeyDown('p'); break;
        case Q_KEYCODE: if(shift) handler->OnKeyDown('Q'); else handler->OnKeyDown('q'); break;
        case R_KEYCODE: if(shift) handler->OnKeyDown('R'); else handler->OnKeyDown('r'); break;
        case S_KEYCODE: if(shift) handler->OnKeyDown('S'); else handler->OnKeyDown('s'); break;
        case T_KEYCODE: if(shift) handler->OnKeyDown('T'); else handler->OnKeyDown('t'); break;
        case U_KEYCODE: if(shift) handler->OnKeyDown('U'); else handler->OnKeyDown('u'); break;
        case V_KEYCODE: if(shift) handler->OnKeyDown('V'); else handler->OnKeyDown('v'); break;
        case W_KEYCODE: if(shift) handler->OnKeyDown('W'); else handler->OnKeyDown('w'); break;
        case X_KEYCODE: if(shift) handler->OnKeyDown('X'); else handler->OnKeyDown('x'); break;
        case Y_KEYCODE: if(shift) handler->OnKeyDown('Y'); else handler->OnKeyDown('y'); break;
        case Z_KEYCODE: if(shift) handler->OnKeyDown('Z'); else handler->OnKeyDown('z'); break;
        case LEFT_SHIFT_DOWN_KEYCODE: case RIGHT_SHIFT_DOWN_KEYCODE: shift = true; break;
        case LEFT_SHIFT_UP_KEYCODE: case RIGHT_SHIFT_UP_KEYCODE: shift = false; break;
        case LEFT_DOT_KEYCODE: if(shift) handler->OnKeyDown('>'); else handler->OnKeyDown('.'); break;
        case OPEN_BRACKET_KEYCODE: if(shift) handler->OnKeyDown('{'); else handler->OnKeyDown('['); break;
        case CLOSE_BRACKET_KEYCODE: if(shift) handler->OnKeyDown('}'); else handler->OnKeyDown(']'); break;
        case NUMPAD_DOT_KEYCODE: handler->OnKeyDown('.'); break;
        case SEMICOLON_KEYCODE: handler->OnKeyDown(';'); break;
        case SINGLE_QOUTE_KEYCODE: handler->OnKeyDown('\''); break;
        case ZERO_KEYCODE: if(shift) handler->OnKeyDown(')'); else handler->OnKeyDown('0'); break;
        case ONE_KEYCODE: if(shift) handler->OnKeyDown('!'); else handler->OnKeyDown('1'); break;
        case TWO_KEYCODE: if(shift) handler->OnKeyDown('@'); else handler->OnKeyDown('2'); break;
        case THREE_KEYCODE: if(shift) handler->OnKeyDown('#'); else handler->OnKeyDown('3'); break;
        case FOUR_KEYCODE: if(shift) handler->OnKeyDown('$'); else handler->OnKeyDown('4'); break;
        case FIVE_KEYCODE: if(shift) handler->OnKeyDown('%'); else handler->OnKeyDown('5'); break;
        case SIX_KEYCODE: if(shift) handler->OnKeyDown('^'); else handler->OnKeyDown('6'); break;
        case SEVEN_KEYCODE: if(shift) handler->OnKeyDown('&'); else handler->OnKeyDown('7'); break;
        case EIGHT_KEYCODE: if(shift) handler->OnKeyDown('*'); else handler->OnKeyDown('8'); break;
        case NINE_KEYCODE: if(shift) handler->OnKeyDown('('); else handler->OnKeyDown('9'); break;
        case ZERO_NUMPAD_KEYCODE: handler->OnKeyDown('0'); break;
        case ONE_NUMPAD_KEYCODE: handler->OnKeyDown('1'); break;
        case TWO_NUMPAD_KEYCODE: handler->OnKeyDown('2'); break;
        case THREE_NUMPAD_KEYCODE: handler->OnKeyDown('3'); break;
        case FOUR_NUMPAD_KEYCODE: handler->OnKeyDown('4'); break;
        case FIVE_NUMPAD_KEYCODE: handler->OnKeyDown('5'); break;
        case SIX_NUMPAD_KEYCODE: handler->OnKeyDown('6'); break;
        case SEVEN_NUMPAD_KEYCODE: handler->OnKeyDown('7'); break;
        case EIGHT_NUMPAD_KEYCODE: handler->OnKeyDown('8'); break;
        case NINE_NUMPAD_KEYCODE: handler->OnKeyDown('9'); break;
        case LEFT_MINUS_KEYCODE: if(shift) handler->OnKeyDown('_'); else handler->OnKeyDown('-'); break;
        case LEFT_EQUAL_KEYCODE: if(shift) handler->OnKeyDown('+'); else handler->OnKeyDown('='); break;
        case TAB_KEYCODE: /*tab*/ break;
        case SPACE_KEYCODE: handler->OnKeyDown(' '); break;
        case SLASH_KEYCODE: if(shift) handler->OnKeyDown('?'); else handler->OnKeyDown('/'); break;
        case BACK_SLASH_KEYCODE: if(shift) handler->OnKeyDown('|'); else handler->OnKeyDown('\\'); break;
        case PLUS_KEYCODE: handler->OnKeyDown('+'); break;
        case MULT_KEYCODE: handler->OnKeyDown('*'); break;
        case HAHAHA_KEYCODE: handler->OnKeyDown('`'); break;
        case MINUS_KEYCODE: handler->OnKeyDown('-'); break;

        case 0x45: /*numlock*/ case 0xc5: break;
        default:
            if(key < 0x80){
                printf("KEYBOARD 0x");
                printfHex(key);
                printf("\n");
            }
            break;
    }
    
    return esp;
}