#include "keyboard.h"
#include "kbd.h"


void printf(char*);


KeyboardDriver:: KeyboardDriver(InterruptManager* manager)
: InterruptHandler(0x21, manager),
dataPort(0x60),
commandPort(0x64)
{
    
    while(commandPort.Read() & 0x01)
        dataPort.Read();
    commandPort.Write(0xae); // activate
    commandPort.Write(0x20); // give us your current state
    uint8_t state = (dataPort.Read() | 1) & ~0x10;
    commandPort.Write(0x60); // set state
    dataPort.Write(state);

    dataPort.Write(0xf4); // activate keyboard
}

KeyboardDriver:: ~KeyboardDriver() { }

uint32_t KeyboardDriver::HandleInterrupt(uint32_t esp){
    uint8_t key = dataPort.Read();
    static bool shift = false;
    switch (key)
    {
        case 0xfa:
            printf("[OK] Keyboard Attached.\n");
            break;
        case A_KEYCODE: if(shift) printf("A"); else printf("a"); break;
        case B_KEYCODE: if(shift) printf("B"); else printf("b"); break;
        case C_KEYCODE: if(shift) printf("C"); else printf("c"); break;
        case D_KEYCODE: if(shift) printf("D"); else printf("d"); break;
        case E_KEYCODE: if(shift) printf("E"); else printf("e"); break;
        case F_KEYCODE: if(shift) printf("F"); else printf("f"); break;
        case G_KEYCODE: if(shift) printf("G"); else printf("g"); break;
        case H_KEYCODE: if(shift) printf("H"); else printf("h"); break;
        case I_KEYCODE: if(shift) printf("I"); else printf("i"); break;
        case J_KEYCODE: if(shift) printf("J"); else printf("j"); break;
        case K_KEYCODE: if(shift) printf("K"); else printf("k"); break;
        case L_KEYCODE: if(shift) printf("L"); else printf("l"); break;
        case M_KEYCODE: if(shift) printf("M"); else printf("m"); break;
        case N_KEYCODE: if(shift) printf("N"); else printf("n"); break;
        case O_KEYCODE: if(shift) printf("O"); else printf("o"); break;
        case P_KEYCODE: if(shift) printf("P"); else printf("p"); break;
        case Q_KEYCODE: if(shift) printf("Q"); else printf("q"); break;
        case R_KEYCODE: if(shift) printf("R"); else printf("r"); break;
        case S_KEYCODE: if(shift) printf("S"); else printf("s"); break;
        case T_KEYCODE: if(shift) printf("T"); else printf("t"); break;
        case U_KEYCODE: if(shift) printf("U"); else printf("u"); break;
        case V_KEYCODE: if(shift) printf("V"); else printf("v"); break;
        case W_KEYCODE: if(shift) printf("W"); else printf("w"); break;
        case X_KEYCODE: if(shift) printf("X"); else printf("x"); break;
        case Y_KEYCODE: if(shift) printf("Y"); else printf("y"); break;
        case Z_KEYCODE: if(shift) printf("Z"); else printf("z"); break;
        case LEFT_SHIFT_DOWN_KEYCODE: case RIGHT_SHIFT_DOWN_KEYCODE: shift = true; break;
        case LEFT_SHIFT_UP_KEYCODE: case RIGHT_SHIFT_UP_KEYCODE: shift = false; break;
        case LEFT_DOT_KEYCODE: if(shift) printf(">"); else printf("."); break;
        case OPEN_BRACKET_KEYCODE: if(shift) printf("{"); else printf("["); break;
        case CLOSE_BRACKET_KEYCODE: if(shift) printf("}"); else printf("]"); break;
        case NUMPAD_DOT_KEYCODE: printf("."); break;
        case SEMICOLON_KEYCODE: printf(";"); break;
        case SINGLE_QOUTE_KEYCODE: printf("'"); break;
        case ZERO_KEYCODE: if(shift) printf(")"); else printf("0"); break;
        case ONE_KEYCODE: if(shift) printf("!"); else printf("1"); break;
        case TWO_KEYCODE: if(shift) printf("@"); else printf("2"); break;
        case THREE_KEYCODE: if(shift) printf("#"); else printf("3"); break;
        case FOUR_KEYCODE: if(shift) printf("$"); else printf("4"); break;
        case FIVE_KEYCODE: if(shift) printf("%"); else printf("5"); break;
        case SIX_KEYCODE: if(shift) printf("^"); else printf("6"); break;
        case SEVEN_KEYCODE: if(shift) printf("&"); else printf("7"); break;
        case EIGHT_KEYCODE: if(shift) printf("*"); else printf("8"); break;
        case NINE_KEYCODE: if(shift) printf("("); else printf("9"); break;
        case ZERO_NUMPAD_KEYCODE: printf("0"); break;
        case ONE_NUMPAD_KEYCODE: printf("1"); break;
        case TWO_NUMPAD_KEYCODE: printf("2"); break;
        case THREE_NUMPAD_KEYCODE: printf("3"); break;
        case FOUR_NUMPAD_KEYCODE: printf("4"); break;
        case FIVE_NUMPAD_KEYCODE: printf("5"); break;
        case SIX_NUMPAD_KEYCODE: printf("6"); break;
        case SEVEN_NUMPAD_KEYCODE: printf("7"); break;
        case EIGHT_NUMPAD_KEYCODE: printf("8"); break;
        case NINE_NUMPAD_KEYCODE: printf("9"); break;
        case LEFT_MINUS_KEYCODE: if(shift) printf("_"); else printf("-"); break;
        case LEFT_EQUAL_KEYCODE: if(shift) printf("+"); else printf("="); break;
        case TAB_KEYCODE: /*tab*/ break;
        case SPACE_KEYCODE: printf(" "); break;
        case SLASH_KEYCODE: if(shift) printf("?"); else printf("/"); break;
        case BACK_SLASH_KEYCODE: if(shift) printf("|"); else printf("\\"); break;
        case PLUS_KEYCODE: printf("+"); break;
        case MULT_KEYCODE: printf("*"); break;
        case HAHAHA_KEYCODE: printf("`"); break;
        case MINUS_KEYCODE: printf("-"); break;

        case 0x45: /*numlock*/ case 0xc5: break;
        default:
            if(key < 0x80){
                char* hex = "0123456789abcdef";
                char* text = "KEYBOARD 0x00\n";
                text[11] = hex[(key>>4) & 0x0f];
                text[12] = hex[ key & 0x0f];
                printf(text);
            }
            break;
    }
    
    return esp;
}