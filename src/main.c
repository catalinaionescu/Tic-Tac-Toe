#define F_CPU 20000000UL
#include <avr/io.h>
#include <util/delay.h>

#define PLAYER 1
#define COMPUTER 2
#define DEBOUNCE_DELAY 200 

int board[3][3];
int current_selection = 0;
int player_turn = PLAYER;

void init() {
    DDRA = 0xFF; 
    DDRB = 0xFF; 
    DDRC = 0x03; 
    DDRD |= (1 << PD7); 

    PORTD |= (1 << PD7); 

    PORTA = 0x00;
    PORTB = 0x00;
    PORTC = 0x00;

    DDRD &= ~((1 << PD3) | (1 << PD5) | (1 << PD6));
    PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
}

void clear_board() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = 0;
        }
    }
}

void set_led(int row, int col, int state, int color) {
    uint8_t pin_green = 0;
    uint8_t pin_red = 0;

    if (row == 0 && col == 0) { pin_green = PA0; pin_red = PA1; }
    else if (row == 0 && col == 1) { pin_green = PA2; pin_red = PA3; }
    else if (row == 0 && col == 2) { pin_green = PA4; pin_red = PA5; }
    else if (row == 1 && col == 0) { pin_green = PA6; pin_red = PA7; }
    else if (row == 1 && col == 1) { pin_green = PB0; pin_red = PB1; }
    else if (row == 1 && col == 2) { pin_green = PB2; pin_red = PB3; }
    else if (row == 2 && col == 0) { pin_green = PB4; pin_red = PB5; }
    else if (row == 2 && col == 1) { pin_green = PB6; pin_red = PB7; }
    else if (row == 2 && col == 2) { pin_green = PC0; pin_red = PC1; }

    if (color == PLAYER) {
        if (state) {
            if (row < 1 || (row == 1 && col == 0)) PORTA |= (1 << pin_green);
            else if (row == 1 || (row == 2 && col < 2)) PORTB |= (1 << pin_green);
            else PORTC |= (1 << pin_green);
        } else {
            if (row < 1 || (row == 1 && col == 0)) PORTA &= ~(1 << pin_green);
            else if (row == 1 || (row == 2 && col < 2)) PORTB &= ~(1 << pin_green);
            else PORTC &= ~(1 << pin_green);
        }
    } else if (color == COMPUTER) {
        if (state) {
            if (row < 1 || (row == 1 && col == 0)) PORTA |= (1 << pin_red);
            else if (row == 1 || (row == 2 && col < 2)) PORTB |= (1 << pin_red);
            else PORTC |= (1 << pin_red);
        } else {
            if (row < 1 || (row == 1 && col == 0)) PORTA &= ~(1 << pin_red);
            else if (row == 1 || (row == 2 && col < 2)) PORTB &= ~(1 << pin_red);
            else PORTC &= ~(1 << pin_red);
        }
    }
}

void display_board() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == PLAYER) set_led(i, j, 1, PLAYER);
            else if (board[i][j] == COMPUTER) set_led(i, j, 1, COMPUTER);
            else set_led(i, j, 0, 0); 
        }
    }
}

void move_selection_left() {
    current_selection = (current_selection > 0) ? current_selection - 1 : 8;
}

void move_selection_right() {
    current_selection = (current_selection < 8) ? current_selection + 1 : 0;
}

void place_move(int player) {
    int row = current_selection / 3;
    int col = current_selection % 3;
    if (board[row][col] == 0) {
        board[row][col] = player;
        player_turn = (player_turn == PLAYER) ? COMPUTER : PLAYER;
    }
}

void computer_move() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) {
                board[i][j] = COMPUTER;
                return;
            }
        }
    }
}

int check_winner() {
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][1] == board[i][2] && board[i][0] != 0) return board[i][0];
        if (board[0][i] == board[1][i] && board[1][i] == board[2][i] && board[0][i] != 0) return board[0][i];
    }
    if (board[0][0] == board[1][1] && board[1][1] == board[2][2] && board[0][0] != 0) return board[0][0];
    if (board[0][2] == board[1][1] && board[1][1] == board[2][0] && board[0][2] != 0) return board[0][2];
    return 0;
}

void blink_winner(int winner) {
    int color = (winner == PLAYER) ? PLAYER : COMPUTER;
    for (int i = 0; i < 5; i++) {
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) set_led(row, col, 1, color);
        }
        _delay_ms(200);
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 3; col++) set_led(row, col, 0, 0);
        }
        _delay_ms(200);
    }
}

void game_loop() {
    int blink_counter = 0;
    int led_state = 0;

    while (1) {
        display_board();
        
    
        int row = current_selection / 3;
        int col = current_selection % 3;
        
        if (board[row][col] == 0) {
            if (led_state) {
                set_led(row, col, 1, PLAYER);
            } else {
                set_led(row, col, 0, 0);
            }
        }

 
        blink_counter++;
        if (blink_counter >= 20) { 
            led_state = !led_state;
            blink_counter = 0;
        }


        if ((PIND & (1 << PD3)) == 0) { 
            _delay_ms(DEBOUNCE_DELAY); 
            move_selection_left(); 
        }
        if ((PIND & (1 << PD5)) == 0) { 
            _delay_ms(DEBOUNCE_DELAY); 
            move_selection_right(); 
        }
        if ((PIND & (1 << PD6)) == 0) { 
            _delay_ms(DEBOUNCE_DELAY); 
            place_move(player_turn);
            
            int winner = check_winner();
            if (winner != 0) { 
                blink_winner(winner); 
                clear_board(); 
                player_turn = PLAYER; 
                current_selection = 0; 
            }
        }

        if (player_turn == COMPUTER) {
            computer_move();
            player_turn = PLAYER;
            
            int winner = check_winner();
            if (winner != 0) { 
                blink_winner(winner); 
                clear_board(); 
                player_turn = PLAYER; 
                current_selection = 0; 
            }
        }

        _delay_ms(10); 
    }
}

int main(void) {
    init();
    clear_board();
    game_loop();
    return 0;
}