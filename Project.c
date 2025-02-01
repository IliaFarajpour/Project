#include <stdio.h>
#include <string.h>
#include <ncurses.h>
#include <ctype.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <locale.h>
#include <unistd.h>

int difficulty = 2; 
char hero_color[10] = "Green"; 
int floornumber = 0;
int firstplace;
bool wasinroom[4][8]={false};
bool roomdrawn[4][8]= {false};
bool coridordrawn1[4][8]={false};
bool corridordrawn2[7]={false, false,false, false, false,false,false};
int doornumber[8]={1,3,2,2,3,2,2,1};
int doornumber2[8]={1,2,1,3,3,1,2,1};
bool coridor[10][20]={false};



#define MAX_LEN 50
#define FILE_NAME "users.txt"
#define SCORES_FILE "scores.txt"
#define MAX_players 150
#define MAX_FOOD 5
#define HUNGER_THRESHOLD 10 
#define MAX_WEAPONS 10

typedef struct {
    char username[MAX_LEN];
    char password[MAX_LEN];
    char email[MAX_LEN];
} User;

typedef struct {
    int rank;               
    char username[MAX_LEN];   
    int score;             
    int gold;                
    int games_completed;      
    long experience_time;     
} Score;
typedef struct{
    int positionx;
    int positiony;
    bool collected;

}food;
int afterhunger=0;
typedef struct {
    int positionx;
    int positiony;
    int width, height;
    int doorpositionx[4];
    int doorpositiony[4];
    int stairpositionx;
    int stairpositiony;
    int foodspositionx[7];
    int foodspositiony[7];
    int goldpositionx[100]; 
    int goldpositiony[100]; 
    int goldcount; 
    int blackgoldpositionx[100];
    int blackgoldpositiony[100]; 
    int blackgoldcount; 
    int foodcount;
    int weaponcount;     
    int weaponpositionx[MAX_WEAPONS];
    int weaponpositiony[MAX_WEAPONS];
    int weaponTypes[MAX_WEAPONS];  
    int arrowcount; 
    int arrowpositionx[10]; 
    int arrowpositiony[10]; 
    int magicwandcount;             
    int magicwandpositionx[10];      
    int magicwandpositiony[10]; 
     food foods[7];
} Room;

typedef struct{
    int positionx;
    int positiony;
    int whichroom;
    int health;
    int food_count;  
    int hunger_timer;
    int hunger;

} adventurer;
typedef struct {
    int gold; 
int food;
 
} Player;


Player player = {0};  
float health=10;
float maxhealth=10;
float hunger=0;
adventurer player1;
int playerx;
int playery;
int foodscollected=0;
int food_count = 0; 
char selected_music[100] = "default_music.mp3"; 
int music_enabled = 1; 
bool hungermessage= false; 

void print_menu(WINDOW *menu, int highlight, char *choices[], int n_choices);
void create_user();
void user_login(); 
int is_valid_email(const char *email);
int is_valid_password(const char *password);
int is_username_taken(const char *username);
int check_user_credentials(const char *username, const char *password);
void pre_game_menu();
void display_scores();
void settings_menu(); 
void change_difficulty();
void change_hero_color();
void start();
void playerplacement(adventurer player1, Room * rooms[], int firstplace);
int checkoverlap(Room *newroom, Room *rooms[], int numrooms);
Room * creatroom(int section, Room * rooms[], int numrooms, int roomnumber);
int drawdoor(Room * room, int roomnumber);
int drawroom (Room *room, int roomnumber);
int connectrooms(Room*rooms[], int numrooms, int firstroom, int secondroom);
void handleinput(adventurer player1, Room *rooms[], int firstplacen, int numrooms ,int stairroom);
void show_full_map(Room *rooms[], int num_rooms);
void change_music(); 
void foodmenu(Room* rooms[], int num_rooms);
void show_attributes(); 
void game_over_screen(); 
void victory_screen(int total_gold); 
void show_temporary_message(int y, int x,const char *message);
 


void generate_random_password(char *password, int length) {
    const char *lowercase = "abcdefghijklmnopqrstuvwxyz";
    const char *uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    const char *digits = "0123456789";
    const char *charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";

    srand(time(NULL));  

    
    password[0] = lowercase[rand() % strlen(lowercase)];  
    password[1] = uppercase[rand() % strlen(uppercase)]; 
    password[2] = digits[rand() % strlen(digits)];       

    for (int i = 3; i < length - 1; i++) {
        password[i] = charset[rand() % strlen(charset)];
    }
    password[length - 1] = '\0';
}

void print_menu(WINDOW *win, int highlight, char *choices[], int n_choices) {
    int x = 2, y = 2;
    for (int i = 0; i < n_choices; ++i) {
        if (highlight == i + 1) {
            wattron(win, A_REVERSE);  
        }
        mvwprintw(win, y, x, choices[i]);
        wattroff(win, A_REVERSE);
        ++y;
    }
    wrefresh(win);
}

void change_difficulty() {
    WINDOW *diff_win;
    int difficulty_choice = difficulty;
    int c;
    char *difficulty_options[] = {
        "Easy",
        "Medium",
        "Hard"
    };

    diff_win = newwin(7, 30, 10, 35);
    keypad(diff_win, TRUE);
    mvwprintw(diff_win, 1, 1, "Select Difficulty:");
    refresh();
    
    while (1) {
        for (int i = 0; i < 3; i++) {
            if (difficulty_choice == i + 1) {
                wattron(diff_win, A_REVERSE);
            }
            mvwprintw(diff_win, 2 + i, 1, difficulty_options[i]);
            wattroff(diff_win, A_REVERSE);
        }
        
        c = wgetch(diff_win);
        if (c == KEY_UP && difficulty_choice > 1) {
            difficulty_choice--;
        } else if (c == KEY_DOWN && difficulty_choice < 3) {
            difficulty_choice++;
        } else if (c == 10) {
            difficulty = difficulty_choice;  
            break;
        }
    }
    clear();
    refresh();
}

void change_hero_color() {
    WINDOW *color_win;
    int color_choice = 1; 
    int c;
    char *color_options[] = {
        "Green",
        "Red",
        "Blue",
        "Yellow"
    };

    color_win = newwin(7, 30, 10, 35);
    keypad(color_win, TRUE);
    mvwprintw(color_win, 1, 1, "Select Hero Color:");
    refresh();

    while (1) {
        for (int i = 0; i < 4; i++) {
            if (color_choice == i + 1) {
                wattron(color_win, A_REVERSE); 
            }
            mvwprintw(color_win, 2 + i, 1, color_options[i]);
            wattroff(color_win, A_REVERSE);  
        }

        c = wgetch(color_win);
        if (c == KEY_UP && color_choice > 1) {
            color_choice--;
        } else if (c == KEY_DOWN && color_choice < 4) {
            color_choice++;
        } else if (c == 10) { 
            switch (color_choice) {
                case 1: strcpy(hero_color, "Green"); break;
                case 2: strcpy(hero_color, "Red"); break;
                case 3: strcpy(hero_color, "Blue"); break;
                case 4: strcpy(hero_color, "Yellow"); break;
            }
            break;  
        }
    }

 
    clear();
    refresh();
}

void settings_menu() {
    WINDOW *settings_win;
    int height = 10, width = 50;
    int starty = 25, startx = 85;
    int choice = 0, highlight = 1;
    int c;
    char *settings_choices[] = {
        "Change Difficulty",
        "Change Hero Color",
        "Change Music",      
        "Back to Main Menu"
    };

    settings_win = newwin(height, width, starty, startx);
    keypad(settings_win, TRUE);
    refresh();

    while (1) {
        wattron(settings_win, COLOR_PAIR(1));
        wborder(settings_win, '#', '#', '#', '#', '#', '#', '#', '#');
        wattroff(settings_win, COLOR_PAIR(1));
        print_menu(settings_win, highlight, settings_choices, 4);
        
        c = wgetch(settings_win);
        if (c == KEY_UP) {
            if (highlight == 1) {
                highlight = 4; 
            } else {
                --highlight;
            }
        } else if (c == KEY_DOWN) {
            if (highlight == 4) {
                highlight = 1;  
            } else {
                ++highlight;
            }
        } else if (c == 10) {
            choice = highlight;

            if (choice == 1) {
                change_difficulty();  
            } else if (choice == 2) {
                change_hero_color();  
            } else if (choice == 3) {
                change_music(); 
            } else if (choice == 4) {
                break; 
            }
        }

        clear();
        refresh();
    }
}

void main_menu() {
    WINDOW *menu;
    health =10;
    
    initscr();            
    start_color();        
    clear();
    noecho();
    curs_set(0);          

    init_pair(1, COLOR_YELLOW, COLOR_BLACK); 

    char *choices[] = {
        "Create a new user",
        "User login",         
        "Pre-game Menu",
        "Scoreboard",
        "Settings"
    };
    int n_choices = 5;

    int height = 10, width = 50;  
    int starty = 25;
    int startx = 85; 

    int highlight = 1; 
    int choice = 0;    
    int c;

    menu = newwin(height, width, starty, startx);
    keypad(menu, TRUE);  
    refresh();

   
    while (1) {
        wattron(menu, COLOR_PAIR(1)); 
   
        wborder(menu, '#', '#', '#', '#', '#', '#', '#', '#'); 
        wattroff(menu, COLOR_PAIR(1));
        print_menu(menu, highlight, choices, n_choices);

        c = wgetch(menu);
        if (c == KEY_UP) { 
            if (highlight == 1) {
                highlight = n_choices;
            } else {
                --highlight;
            }
        } else if (c == KEY_DOWN) {
            if (highlight == 5) {
                highlight = 5;
            } else {
                ++highlight;
            }
        } 
        else if (c == 10) {  
            choice = highlight;

            if (choice == 1) {  
                create_user();
            } else if (choice == 2) {  
                user_login();
            }
            else if (choice == 3) {  
                pre_game_menu();
            }
            else if (choice == 4) {  
                printw("Displaying scoreboard...\n");
            }
            else  {
                clear();  
                settings_menu();  
            }

           
            clear();
            refresh();
        }
    }

    endwin();
}

int is_username_taken(const char *username) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        return 0; 
    }

    char line[MAX_LEN * 3];
    while (fgets(line, sizeof(line), file)) {
        
        char *saved_username = strtok(line, ",");
        if (saved_username != NULL && strcmp(saved_username, username) == 0) {
            fclose(file);
            return 1; 
        }
    }

    fclose(file);
    return 0; 
}

int find_password_by_email(const char *email, char *password) {
    FILE *file = fopen(FILE_NAME, "r");
    if (!file) {
        return -1;
    }

    char stored_username[MAX_LEN], stored_password[MAX_LEN], stored_email[MAX_LEN];
    
   
    while (fscanf(file, "%[^,],%[^,],%s\n", stored_username, stored_password, stored_email) != EOF) {
    
        if (strcmp(email, stored_email) == 0) {
            strcpy(password, stored_password);  
            fclose(file);
            return 1;
        }
    }
    
    fclose(file);
    return 0; 
}

void create_user() {
    getch();
    clear();
    refresh();
    User new_user;
    clear();
    mvprintw(12, 80, "Enter username: ");
    echo();
    getstr(new_user.username);
    noecho();

    if (is_username_taken(new_user.username)) {
        mvprintw(14, 80, "Username already taken! Please choose another.");
        getch();
        return;  
    }

   
    int valid_password = 0;
    while (!valid_password) {
         mvprintw(16, 80, "Enter password or type 'random' for a random password: ");
        echo();
        getstr(new_user.password);
        noecho();
         if (strcmp(new_user.password, "random") == 0) {
            generate_random_password(new_user.password, 12);  
            mvprintw(18, 80, "Generated random password: %s", new_user.password);
            clrtoeol();
        }

        if (is_valid_password(new_user.password)) {
            valid_password = 1; 
        } else {
            mvprintw(18, 80, "Invalid password! Must be at least 7 chars, 1 uppercase, 1 lowercase, and 1 digit.");
            clrtoeol();
        }
    }

    int valid_email = 0;
    while (!valid_email) {
        mvprintw(20, 80, "Enter email: ");
        echo();
        getstr(new_user.email);
        noecho();

        if (is_valid_email(new_user.email)) {
            valid_email = 1; 
        } else {
            mvprintw(22, 80, "Invalid email format! Example: xxx@y.zzz");
            clrtoeol(); 
        }
    }

    FILE *file = fopen(FILE_NAME, "a");
    if (file == NULL) {
        mvprintw(10, 80, "Error opening file for saving user.");
        getch();
        return;
    }
    fprintf(file, "%s,%s,%s\n", new_user.username, new_user.password, new_user.email);
    fclose(file);

    mvprintw(24, 80, "User created successfully! Press any key to return to menu.");
    getch();

    clear();  
    refresh();  
}

int is_valid_email(const char *email) {
    const char *at = strchr(email, '@');
    if (at == NULL || at == email) return 0; 

    const char *dot = strchr(at, '.');
    if (dot == NULL || dot == at + 1) return 0;

    return 1;
}

int is_valid_password(const char *password) {
    int has_upper = 0, has_lower = 0, has_digit = 0;

    if (strlen(password) < 7) return 0;
    for (int i = 0; password[i] != '\0'; i++) {
        if (password[i] >= 'A' && password[i] <= 'Z') {
            has_upper = 1;
        }
        if (password[i] >= 'a' && password[i] <= 'z') {
            has_lower = 1;
        }
        if (password[i] >= '0' && password[i] <= '9') {
            has_digit = 1;
        }
    }

    return has_upper && has_lower && has_digit; 
}

int check_user_credentials(const char *username, const char *password) {
    FILE *file = fopen(FILE_NAME, "r");
    if (file == NULL) {
        return -1;
    }

    char line[MAX_LEN * 3];
    int username_found = 0;

    while (fgets(line, sizeof(line), file)) {
        
        char *file_username = strtok(line, ",");
        char *file_password = strtok(NULL, ",");
        char *file_email = strtok(NULL, ",");

        if (file_username && strcmp(username, file_username) == 0) {
            username_found = 1; 

            if (file_password && strcmp(password, file_password) == 0) {
                fclose(file);
                return 1; 
            }
        }
    }

    fclose(file);
    return username_found ? 0 : -1;
}

void user_login() {
    char username[MAX_LEN], password[MAX_LEN], email[MAX_LEN];
    int status;
    clear();

    mvprintw(12, 80, "Enter username (or press 'g' for guest login): ");
    echo();
    getstr(username);
    noecho();

    if (strcmp(username, "g") == 0) {
        clear();
        mvprintw(14, 80, "Logged in as Guest. ");
        mvprintw(16, 80, "Press Enter to continue to the main menu.");
        while (getch() != '\n');  
        main_menu();  
    }

   
    mvprintw(14, 80, "Enter password (Forgot your password? Press 'f' to reset.) : ");
    echo();
    getstr(password);
    noecho();

  
    if (strcmp(password, "f") == 0 || strcmp(password, "F") == 0) {
        clear();
        mvprintw(14, 80, "Enter your email to receive your password: ");
        echo();
        getstr(email);
        noecho();

      
        char recovered_password[MAX_LEN];
        int result = find_password_by_email(email, recovered_password);

        if (result == 1) {
            mvprintw(16, 80, "Password for %s is: %s", email, recovered_password);
        } else {
            mvprintw(16, 80, "Email not found!");
        }
        getch();
        clear();
        refresh();
        return; 
    }

 
    status = check_user_credentials(username, password);

    if (status == 1) {
        mvprintw(16, 80, "Login successful! Press any key to continue.");
    } else if (status == 0) {
        mvprintw(16, 80, "Incorrect password! Please try again.");
    } else if (status == -1) {
        mvprintw(16, 80, "Username not found! Please register first.");
    }

    getch(); 
    clear(); 
    refresh(); 
}

void pre_game_menu() {
    WINDOW *menu;
    int height = 12, width = 50;  
    int starty = 25; 
    int startx = 85; 
    int highlight = 1, choice = 0, c;

    char *choices[] = {
        "New Game", 
        "Continue Game",
        "Back to Main Menu"
    };
    int n_choices =3;

    
    menu = newwin(height, width, starty, startx);
    keypad(menu, TRUE); 
    refresh();

    while (1) {
       
        wattron(menu, COLOR_PAIR(1));
        wborder(menu, '#', '#', '#', '#', '#', '#', '#', '#');
        wattroff(menu, COLOR_PAIR(1));
        print_menu(menu, highlight, choices, n_choices);

        c = wgetch(menu);
        if (c == KEY_UP) {
            if (highlight == 1) {
                highlight = n_choices;
            } else {
                --highlight;
            }
        } else if (c == KEY_DOWN) {
            if (highlight == n_choices) {
                highlight = 1;
            } else {
                ++highlight;
            }
        } else if (c == 10) {  
            choice = highlight;
            break;  
        }
    }

   
    clear();
    switch (choice) {
        case 1:
        start();
            break;
        case 2:
            mvprintw(12, 80, "Continuing the previous game...");
            
            break;
        case 3:
            main_menu();
            break;

    }

    getch();
    clear();
    refresh();
}

void display_scores() {
    FILE *file = fopen(SCORES_FILE, "r");
    if (file == NULL) {
        clear();
        mvprintw(10, 80, "Error opening scores file.");
        getch();
        return;
    }

    Score scores[100];  
    int i = 0;
    while (fscanf(file, "%d,%49s,%d,%d,%d,%ld\n", &scores[i].rank, scores[i].username, 
                  &scores[i].score, &scores[i].gold, &scores[i].games_completed, 
                  &scores[i].experience_time) != EOF) {
        i++;
    }
    fclose(file);

    clear();
    mvprintw(10, 80, "Rank | Username   | Score | Gold | Games | Experience Time");
    mvhline(11, 80, '-', 50);

   
    for (int j = 0; j < i; j++) {
        mvprintw(12 + j, 80, "%-4d | %-10s | %-5d | %-4d | %-5d | %-4ld",
                 scores[j].rank, scores[j].username, scores[j].score, 
                 scores[j].gold, scores[j].games_completed, scores[j].experience_time);
    }

    mvprintw(12 + i, 80, "Press any key to return...");
    getch(); 
    clear();
    refresh();
}

void start() { 
    clear();
    srand(time(NULL));
    initscr();
    noecho();
    curs_set(FALSE);


    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "Mix_OpenAudio failed: %s\n", Mix_GetError());
        return;
    }

  
    Mix_Music *music = NULL;
    if (music_enabled ) {
        music = Mix_LoadMUS(selected_music);
        if (!music) {
            fprintf(stderr, "Mix_LoadMUS failed: %s\n", Mix_GetError());
            return;
        }

        Mix_PlayMusic(music, -1);
    }

   

    mvprintw(50, 100, "Floor: %d", floornumber + 1);
    refresh();

    int numrooms = rand() % 3 + 6;
    Room *rooms[numrooms];
    firstplace = rand() % numrooms;
    for (int i = 0; i < numrooms; i++) {
        rooms[i] = creatroom(i, rooms, i, i);
    }
    drawroom(rooms[firstplace], firstplace);
    roomdrawn[floornumber][firstplace] = true;

    bool placeokforstair = true;
    int stairroom = firstplace;
    while (stairroom == firstplace) {
        stairroom = rand() % numrooms;
    }
    bool stairgenerated = false;
    while (!stairgenerated) {
        int tempx = rooms[stairroom]->positionx + rand() % (rooms[stairroom]->width - 2);
        int tempy = rooms[stairroom]->positiony + rand() % (rooms[stairroom]->height - 2) + 1;

        for (int i = 0; i < doornumber[stairroom]; i++) {
            if (rooms[stairroom]->doorpositionx[i] == tempx && rooms[stairroom]->doorpositiony[i] == tempy) {
                placeokforstair = false;
            }
        }
        if (placeokforstair) {
            rooms[stairroom]->stairpositionx = rooms[stairroom]->positionx + rand() % (rooms[stairroom]->width - 1) + 1;
            rooms[stairroom]->stairpositiony = rooms[stairroom]->positiony + rand() % (rooms[stairroom]->height - 2) + 1;
            stairgenerated = true;
        }
    }

         int foodnumbers= rand()%8+1;
        
        int foodsgenerated=0;
        for (int i =0 ;i<numrooms;i++){
            rooms[i]->foodcount=0;
        }
        
        while (foodsgenerated < foodnumbers)
        {   int foodroom=rand()%numrooms;
            int posx= rooms[foodroom]->positionx+rand()%(rooms[foodroom]->width-2)+1;
            int posy=rooms[foodroom]->positiony+rand()%(rooms[foodroom]->height-2)+1;
            bool placeisokforfood = true;
            for (int i =0 ; i < doornumber[foodroom];i++){
            if(rooms[foodroom]->doorpositionx[i] == posx && rooms[foodroom]->doorpositiony[i] == posy){
                placeisokforfood=false;
            }}
            if(rooms[foodroom]->stairpositionx == posx && rooms[foodroom]->stairpositiony == posy){
             placeisokforfood=false;

            }
           if (playerx==posx && playery==posy){
               placeisokforfood=false;          
           }
            
           if (placeisokforfood){
            rooms[foodroom]->foods[rooms[foodroom]->foodcount].positionx= posx;
             rooms[foodroom]->foods[rooms[foodroom]->foodcount].positiony=posy;
             rooms[foodroom]->foods[rooms[foodroom]->foodcount].collected = false;
             rooms[foodroom]->foodcount++;
            
            

            foodsgenerated++;
           }
        }
 int goldnumbers = rand() % 3 + 1;
 int blackgoldnumbers = rand() % 2 + 1;  
 int goldsgenerated = 0;
 int blackgoldsgenerated = 0;

 while (goldsgenerated < goldnumbers) {
    int goldroom = rand() % numrooms; 
    int posx = rooms[goldroom]->positionx + rand() % (rooms[goldroom]->width - 2) + 1;
    int posy = rooms[goldroom]->positiony + rand() % (rooms[goldroom]->height - 2) + 1;

    bool placeisokforgold = true;
    for (int i = 0; i < doornumber[goldroom]; i++) {
        if (rooms[goldroom]->doorpositionx[i] == posx && rooms[goldroom]->doorpositiony[i] == posy) {
            placeisokforgold = false;
        }
    }
    if (rooms[goldroom]->stairpositionx == posx && rooms[goldroom]->stairpositiony == posy) {
        placeisokforgold = false;
    }
    for (int i = 0; i < rooms[goldroom]->foodcount; i++) {
        if (rooms[goldroom]->foodspositionx[i] == posx && rooms[goldroom]->foodspositiony[i] == posy) {
            placeisokforgold = false;
        }
    }

    if (placeisokforgold) {
        rooms[goldroom]->goldpositionx[rooms[goldroom]->goldcount] = posx;
        rooms[goldroom]->goldpositiony[rooms[goldroom]->goldcount] = posy;
        rooms[goldroom]->goldcount++;
        goldsgenerated++;
    }
 } 


 while (blackgoldsgenerated < blackgoldnumbers) {
    int blackgoldroom = rand() % numrooms;
    int posx = rooms[blackgoldroom]->positionx + rand() % (rooms[blackgoldroom]->width - 2) + 1;
    int posy = rooms[blackgoldroom]->positiony + rand() % (rooms[blackgoldroom]->height - 2) + 1;

    bool placeisokforblackgold = true;
    for (int i = 0; i < doornumber[blackgoldroom]; i++) {
        if (rooms[blackgoldroom]->doorpositionx[i] == posx && rooms[blackgoldroom]->doorpositiony[i] == posy) {
            placeisokforblackgold = false;
        }
    }
    if (rooms[blackgoldroom]->stairpositionx == posx && rooms[blackgoldroom]->stairpositiony == posy) {
        placeisokforblackgold = false;
    }
    for (int i = 0; i < rooms[blackgoldroom]->foodcount; i++) {
        if (rooms[blackgoldroom]->foodspositionx[i] == posx && rooms[blackgoldroom]->foodspositiony[i] == posy) {
            placeisokforblackgold = false;
        }
    }

    if (placeisokforblackgold) {
        rooms[blackgoldroom]->blackgoldpositionx[rooms[blackgoldroom]->blackgoldcount] = posx;
        rooms[blackgoldroom]->blackgoldpositiony[rooms[blackgoldroom]->blackgoldcount] = posy;
        rooms[blackgoldroom]->blackgoldcount++;
        blackgoldsgenerated++;
    }
 }
 int weaponNumbers = 5;  
 int weaponsGenerated = 0; 
  while (weaponsGenerated < weaponNumbers) {
    int weaponRoom = rand() % numrooms;
    int posx = rooms[weaponRoom]->positionx + rand() % (rooms[weaponRoom]->width - 2) + 1;
    int posy = rooms[weaponRoom]->positiony + rand() % (rooms[weaponRoom]->height - 2) + 1;

    bool placeIsOkForWeapon = true;

    for (int i = 0; i < doornumber[weaponRoom]; i++) {
        if (rooms[weaponRoom]->doorpositionx[i] == posx && rooms[weaponRoom]->doorpositiony[i] == posy) {
            placeIsOkForWeapon = false;
        }
    }

    
    if (rooms[weaponRoom]->stairpositionx == posx && rooms[weaponRoom]->stairpositiony == posy) {
        placeIsOkForWeapon = false;
    }

    
    for (int i = 0; i < rooms[weaponRoom]->foodcount; i++) {
        if (rooms[weaponRoom]->foodspositionx[i] == posx && rooms[weaponRoom]->foodspositiony[i] == posy) {
            placeIsOkForWeapon = false;
        }
    }

   
    for (int i = 0; i < rooms[weaponRoom]->blackgoldcount; i++) {
        if (rooms[weaponRoom]->blackgoldpositionx[i] == posx && rooms[weaponRoom]->blackgoldpositiony[i] == posy) {
            placeIsOkForWeapon = false;
        }
    }

    if (placeIsOkForWeapon) {
        rooms[weaponRoom]->weaponpositionx[rooms[weaponRoom]->weaponcount] = posx;
        rooms[weaponRoom]->weaponpositiony[rooms[weaponRoom]->weaponcount] = posy;

       
        rooms[weaponRoom]->weaponTypes[rooms[weaponRoom]->weaponcount] = rand() % 4; 

        rooms[weaponRoom]->weaponcount++;
        weaponsGenerated++;
    }
 }

  int arrowNumbers = 3; 
 int arrowsGenerated = 0; 
  
 while (arrowsGenerated < arrowNumbers) {
    int arrowRoom = rand() % numrooms; 
    int posx = rooms[arrowRoom]->positionx + rand() % (rooms[arrowRoom]->width - 2) + 1;
    int posy = rooms[arrowRoom]->positiony + rand() % (rooms[arrowRoom]->height - 2) + 1;

    bool placeIsOkForArrow = true;

    for (int i = 0; i < doornumber[arrowRoom]; i++) {
        if (rooms[arrowRoom]->doorpositionx[i] == posx && rooms[arrowRoom]->doorpositiony[i] == posy) {
            placeIsOkForArrow = false;
        }
    }

    if (rooms[arrowRoom]->stairpositionx == posx && rooms[arrowRoom]->stairpositiony == posy) {
        placeIsOkForArrow = false;
    }


    for (int i = 0; i < rooms[arrowRoom]->foodcount; i++) {
        if (rooms[arrowRoom]->foodspositionx[i] == posx && rooms[arrowRoom]->foodspositiony[i] == posy) {
            placeIsOkForArrow = false;
        }
    }

    for (int i = 0; i < rooms[arrowRoom]->weaponcount; i++) {
        if (rooms[arrowRoom]->weaponpositionx[i] == posx && rooms[arrowRoom]->weaponpositiony[i] == posy) {
            placeIsOkForArrow = false;
        }
    }


    for (int i = 0; i < rooms[arrowRoom]->blackgoldcount; i++) {
        if (rooms[arrowRoom]->blackgoldpositionx[i] == posx && rooms[arrowRoom]->blackgoldpositiony[i] == posy) {
            placeIsOkForArrow = false;
        }
    }


    if (placeIsOkForArrow) {
        rooms[arrowRoom]->arrowpositionx[rooms[arrowRoom]->arrowcount] = posx;
        rooms[arrowRoom]->arrowpositiony[rooms[arrowRoom]->arrowcount] = posy;

        rooms[arrowRoom]->arrowcount++;
        arrowsGenerated++;
    }
 }
 int magicWandNumbers = 2; 
 int magicWandsGenerated = 0;

 while (magicWandsGenerated < magicWandNumbers) {
    int magicWandRoom = rand() % numrooms;
    int posx = rooms[magicWandRoom]->positionx + rand() % (rooms[magicWandRoom]->width - 2) + 1;
    int posy = rooms[magicWandRoom]->positiony + rand() % (rooms[magicWandRoom]->height - 2) + 1;

    bool placeIsOkForMagicWand = true;


    for (int i = 0; i < doornumber[magicWandRoom]; i++) {
        if (rooms[magicWandRoom]->doorpositionx[i] == posx && rooms[magicWandRoom]->doorpositiony[i] == posy) {
            placeIsOkForMagicWand = false;
        }
    }

  
    if (rooms[magicWandRoom]->stairpositionx == posx && rooms[magicWandRoom]->stairpositiony == posy) {
        placeIsOkForMagicWand = false;
    }

    
    for (int i = 0; i < rooms[magicWandRoom]->foodcount; i++) {
        if (rooms[magicWandRoom]->foodspositionx[i] == posx && rooms[magicWandRoom]->foodspositiony[i] == posy) {
            placeIsOkForMagicWand = false;
        }
    }


    for (int i = 0; i < rooms[magicWandRoom]->weaponcount; i++) {
        if (rooms[magicWandRoom]->weaponpositionx[i] == posx && rooms[magicWandRoom]->weaponpositiony[i] == posy) {
            placeIsOkForMagicWand = false;
        }
    }

  
    for (int i = 0; i < rooms[magicWandRoom]->blackgoldcount; i++) {
        if (rooms[magicWandRoom]->blackgoldpositionx[i] == posx && rooms[magicWandRoom]->blackgoldpositiony[i] == posy) {
            placeIsOkForMagicWand = false;
        }
    }


    if (placeIsOkForMagicWand) {
        rooms[magicWandRoom]->magicwandpositionx[rooms[magicWandRoom]->magicwandcount] = posx;
        rooms[magicWandRoom]->magicwandpositiony[rooms[magicWandRoom]->magicwandcount] = posy;

        rooms[magicWandRoom]->magicwandcount++;
        magicWandsGenerated++; 
    }
 } 
    playerplacement(player1, rooms, firstplace);
    refresh();

    while (1) {
    
       show_attributes(); 
        handleinput(player1, rooms, firstplace, numrooms, stairroom);
    }

    refresh();
    getch();
    endwin();

    Mix_FreeMusic(music);  
    Mix_CloseAudio();      
    SDL_Quit();
}

void playerplacement(adventurer player1, Room *rooms[], int firstplace) {
    adventurer number1;

    player1.positionx = rooms[firstplace]->positionx + rooms[firstplace]->width / 2;
    playerx = rooms[firstplace]->positionx + rooms[firstplace]->width / 2;
    player1.positiony = rooms[firstplace]->positiony + rooms[firstplace]->height / 2;
    playery = rooms[firstplace]->positiony + rooms[firstplace]->height / 2;

   
    if (strcmp(hero_color, "Green") == 0) {
        start_color();
        init_pair(10,COLOR_GREEN,COLOR_BLACK);
        wattron(stdscr, COLOR_PAIR(10));  
    } else if (strcmp(hero_color, "Red") == 0) {
        start_color();
        init_pair(11,COLOR_RED,COLOR_BLACK);
        wattron(stdscr, COLOR_PAIR(11));  
    } else if (strcmp(hero_color, "Blue") == 0) {
        start_color();
        init_pair(12,COLOR_BLUE,COLOR_BLACK);
        wattron(stdscr, COLOR_PAIR(12));  
    } else if (strcmp(hero_color, "Yellow") == 0) {
        start_color();
        init_pair(13,COLOR_YELLOW,COLOR_BLACK);
        wattron(stdscr, COLOR_PAIR(13));  
    }

    mvprintw(player1.positiony, player1.positionx, "@");

    attroff(COLOR_PAIR(10));
    attroff(COLOR_PAIR(11));
    attroff(COLOR_PAIR(12));
    attroff(COLOR_PAIR(13));
}

int checkoverlap(Room * newroom, Room *rooms[], int numrooms){
    for (int i = 0 ; i < numrooms; i++ ){
        if (newroom->positionx < rooms[i]->positionx + rooms[i]->width &&
        newroom->positionx + newroom->width > rooms[i]->positionx &&
        newroom->positiony < rooms[i]->positiony + rooms[i]->height &&
        newroom->positiony + newroom->height > rooms[i]->positiony)
        {
            return 1;
        }
    }
    return 0;
}

Room * creatroom(int section, Room * rooms[], int numrooms, int roomnumber){
    Room * newroom = malloc (sizeof(Room));

    switch (section)
    {
        case 0: newroom->positionx = 5; newroom->positiony = 5; break;
        case 1: newroom->positionx = 45; newroom->positiony = 5; break;
        case 2: newroom->positionx = 85; newroom->positiony = 5; break;
        case 3: newroom->positionx = 5; newroom->positiony = 23; break;
        case 4: newroom->positionx = 45; newroom->positiony = 23; break;
        case 5: newroom->positionx = 85; newroom->positiony = 23; break;
        case 6: newroom->positionx = 5; newroom->positiony = 37;break;
        case 7: newroom->positionx = 45; newroom->positiony = 37; break;

    }
    newroom ->height = rand () % 8 + 5;
    newroom->width = rand () %17 + 6;
    int overlap =1; 
    while ( overlap)
    {
        int randomx= rand () % (29 - newroom->width +1);
        if(randomx < 0)
            {
                randomx =0;
            }
            newroom -> positionx += randomx;
        int randomy= rand () % (12 - newroom->height +1);
         if(randomy < 0)
            {
                randomy =0;
            }
                        newroom -> positiony += randomy;

        overlap = checkoverlap (newroom , rooms , numrooms);

    }
    
        if(roomnumber==0){
             newroom ->doorpositionx[0] = newroom->positionx+ newroom ->width/2;
        newroom->doorpositiony[0]= newroom->positiony + newroom->height - 1;
        
        }
        else if (roomnumber==1){
            newroom ->doorpositionx[0] = newroom ->positionx+newroom ->width/2;
       newroom ->doorpositiony[0]= newroom ->positiony + newroom->height - 1;
     
        newroom ->doorpositionx[1] = newroom ->positionx + newroom ->width -1;
        newroom ->doorpositiony[1]= newroom ->positiony + newroom->height/2;
       

        }
        else if(roomnumber==2){
        newroom ->doorpositionx[0] = newroom ->positionx;
        newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
        }
        else if ( roomnumber == 3){
            newroom->doorpositionx[0] = newroom ->positionx+newroom ->width/2;
       newroom ->doorpositiony[0]= newroom->positiony;
        
         newroom ->doorpositionx[1] = newroom ->positionx + newroom ->width -1;
        newroom ->doorpositiony[1]= newroom ->positiony + newroom->height/2;
       
        newroom ->doorpositionx[2] = newroom ->positionx+ newroom ->width/2;
        newroom ->doorpositiony[2]= newroom ->positiony + newroom->height - 1;
        
        }
        else if (roomnumber == 4){
            newroom ->doorpositionx[0] = newroom ->positionx;
        newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
        newroom->doorpositionx[1] = newroom ->positionx+newroom ->width/2;
       newroom ->doorpositiony[1]= newroom->positiony;
        
        newroom ->doorpositionx[2] = newroom->positionx + newroom ->width -1;
     newroom ->doorpositiony[2]= newroom ->positiony + newroom ->height/2;
       

        }
        else if (roomnumber == 5){
            newroom ->doorpositionx[0] = newroom ->positionx;
        newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
        }
        else if (roomnumber==6){
            newroom->doorpositionx[0] = newroom ->positionx+newroom ->width/2;
       newroom ->doorpositiony[0]= newroom->positiony;
        
         newroom ->doorpositionx[1] = newroom ->positionx + newroom ->width -1;
        newroom ->doorpositiony[1]= newroom ->positiony + newroom->height/2;
       
        }
        else if (roomnumber== 7){
            newroom ->doorpositionx[0] = newroom ->positionx;
        newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
        }
    

    if (roomnumber == 0){
        newroom->doorpositionx[0] = newroom->positionx + newroom->width - 1;
        newroom->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
    }
   else if (roomnumber == 1){
        newroom ->doorpositionx[0] = newroom ->positionx;
        newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
        newroom ->doorpositionx[1] = newroom ->positionx + newroom ->width -1;
        newroom ->doorpositiony[1]= newroom ->positiony + newroom->height/2;
       
        newroom ->doorpositionx[2] = newroom ->positionx+ newroom ->width/2;
        newroom ->doorpositiony[2]= newroom ->positiony + newroom->height - 1;
        
        
    }
   else if (roomnumber ==2 ){
        newroom->doorpositionx[0] = newroom ->positionx;
        newroom ->doorpositiony[0]= newroom ->positiony +newroom ->height/2;
        
        newroom ->doorpositionx[1] = newroom ->positionx+ newroom ->width/2;
       newroom ->doorpositiony[1]= newroom ->positiony + newroom ->height - 1;
        

    }
   else if (roomnumber ==3){
         newroom ->doorpositionx[0] = newroom ->positionx + newroom ->width -1;
       newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
         newroom ->doorpositionx[1] = newroom ->positionx+newroom ->width/2;
       newroom ->doorpositiony[1]= newroom ->positiony + newroom->height - 1;
        
    }
   else if(roomnumber ==4 ){
        newroom ->doorpositionx[0] = newroom ->positionx;
       newroom ->doorpositiony[0]= newroom ->positiony + newroom->height/2;
        
        newroom->doorpositionx[1] = newroom ->positionx+newroom ->width/2;
       newroom ->doorpositiony[1]= newroom->positiony;
        
         newroom ->doorpositionx[2] = newroom->positionx + newroom ->width -1;
     newroom ->doorpositiony[2]= newroom ->positiony + newroom ->height/2;
       
    }
    else if(roomnumber==5){
        newroom ->doorpositionx[0] = newroom ->positionx;
       newroom->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
        
       newroom->doorpositionx[1] = newroom ->positionx+newroom->width/2;
       newroom ->doorpositiony[1]= newroom ->positiony;
        

    }
    else if(roomnumber == 6){
         newroom ->doorpositionx[0] = newroom ->positionx + newroom ->width -1;
        newroom ->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
       
       newroom ->doorpositionx[1] = newroom ->positionx+newroom ->width/2;
      newroom->doorpositiony[1]= newroom ->positiony;
        
    }
    else if (roomnumber == 7){
       newroom ->doorpositionx[0] = newroom->positionx;
        newroom->doorpositiony[0]= newroom ->positiony + newroom ->height/2;
        
    }
    


    return newroom;
    
} 

int drawdoor(Room * room, int roomnumber){
    if (roomnumber == 0){
        room->doorpositionx[0] = room->positionx + room->width - 1;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
    }
    if (roomnumber == 1){
        room->doorpositionx[0] = room->positionx;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
        room->doorpositionx[1] = room->positionx + room->width -1;
        room->doorpositiony[1]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[1],room->doorpositionx[1], "+");
        room->doorpositionx[2] = room->positionx+ room ->width/2;
        room->doorpositiony[2]= room->positiony + room->height - 1;
        mvprintw(room->doorpositiony[2],room->doorpositionx[2], "+");
        
    }
    if (roomnumber ==2 ){
        room->doorpositionx[0] = room->positionx;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
        room->doorpositionx[1] = room->positionx+ room ->width/2;
        room->doorpositiony[1]= room->positiony + room->height - 1;
        mvprintw(room->doorpositiony[1],room->doorpositionx[1], "+");

    }
    if (roomnumber ==3){
         room->doorpositionx[0] = room->positionx + room->width -1;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
         room->doorpositionx[1] = room->positionx+ room ->width/2;
        room->doorpositiony[1]= room->positiony + room->height - 1;
        mvprintw(room->doorpositiony[1],room->doorpositionx[1], "+");
    }
    if(roomnumber ==4 ){
         room->doorpositionx[0] = room->positionx;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
         room->doorpositionx[1] = room->positionx+room->width/2;
        room->doorpositiony[1]= room->positiony;
        mvprintw(room->doorpositiony[1],room->doorpositionx[1], "+");
         room->doorpositionx[2] = room->positionx + room->width -1;
        room->doorpositiony[2]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[2],room->doorpositionx[2], "+");
    }
    if(roomnumber==5){
        room->doorpositionx[0] = room->positionx;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
        room->doorpositionx[1] = room->positionx+room->width/2;
        room->doorpositiony[1]= room->positiony;
        mvprintw(room->doorpositiony[1],room->doorpositionx[1], "+");

    }
    if(roomnumber == 6){
         room->doorpositionx[0] = room->positionx + room->width -1;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
        room->doorpositionx[1] = room->positionx+room->width/2;
        room->doorpositiony[1]= room->positiony;
        mvprintw(room->doorpositiony[1],room->doorpositionx[1], "+");
    }
    if (roomnumber == 7){
        room->doorpositionx[0] = room->positionx;
        room->doorpositiony[0]= room->positiony + room->height/2;
        mvprintw(room->doorpositiony[0],room->doorpositionx[0], "+");
    }
    return 1;

}

int drawroom (Room *room, int roomnumber){
    start_color();
    init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
    attron(COLOR_PAIR(5)); 

    int x , y; 
    for (x = room ->positionx ; x < room -> positionx + room ->width;x++)
    {
        mvprintw(room->positiony, x, "-");
        mvprintw(room->positiony + room->height - 1, x, "-");
    }
    for (y = room ->positiony+1 ; y < room -> positiony + room -> height -1; y++ )
    {
        mvprintw(y, room->positionx, "|");
        mvprintw(y, room->positionx + room->width - 1, "|");
        for (x = room -> positionx +1 ; x < room -> positionx + room -> width -1 ; x++){
                        mvprintw(y, x, ".");

        }
    

    }
    for (int i =0 ; i < doornumber[roomnumber]; i++)
    {   
        mvprintw(room->doorpositiony[i], room->doorpositionx[i], "+");
    }
    attroff(COLOR_PAIR(5));
    return 0;
   
}

int connectrooms(Room*rooms[], int numrooms, int firstroom, int secondroom){

    start_color();
    init_pair(140,COLOR_YELLOW,COLOR_BLACK);
    attron(COLOR_PAIR(140));

    if(numrooms >= 6){

        if (firstroom==0 && secondroom ==1) {
        int x1=rooms[0]->doorpositionx[0]+1;
        int y1 = rooms[0]->doorpositiony[0];
        int x2 = rooms[1]->doorpositionx[0];
        int y2= rooms[1]->doorpositiony[0];
        while(x1 != x2-1){
            mvprintw(y1,x1,"#");
            x1++;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
        else if (y1==y2) {
            mvprintw(y1,x1, "#");
        }
        
            
        
         mvprintw(y2,x2, "#");
        }
        
        if (firstroom==1 && secondroom ==0) {
        int x1=rooms[1]->doorpositionx[0]-1;
        int y1 = rooms[1]->doorpositiony[0];
        int x2 = rooms[0]->doorpositionx[0];
        int y2= rooms[0]->doorpositiony[0];
        while(x1 != x2+1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
        else if (y1==y2) {
            mvprintw(y1,x1, "#");
        }
            
        
         mvprintw(y2,x2, "#");
        }
        
        if (firstroom==1 && secondroom ==2) {
        int x1=rooms[1]->doorpositionx[1]+1;
        int y1 = rooms[1]->doorpositiony[1];
        int x2 = rooms[2]->doorpositionx[0];
        int y2= rooms[2]->doorpositiony[0];
        while(x1 != x2-1){
            mvprintw(y1,x1,"#");
            x1++;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
         else if (y1==y2){
                        mvprintw(y1,x1,"#");

        }
            
        
         mvprintw(y2,x2, "#");
        }
        
        if (firstroom==2 && secondroom ==1) {
        int x1=rooms[2]->doorpositionx[0]-1;
        int y1 = rooms[2]->doorpositiony[0];
        int x2 = rooms[1]->doorpositionx[1];
        int y2= rooms[1]->doorpositiony[1];
        while(x1 != x2+1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
        else if (y1==y2) {
            mvprintw(y1,x1, "#");
        }
            
        
         mvprintw(y2,x2, "#");
        }
         
        if (firstroom==2 && secondroom==4){
            int x1=rooms[1]->doorpositionx[2];
         int y1 = rooms[1]->doorpositiony[2]+1;
         int x2 = rooms[4]->doorpositionx[1];
        int y2= rooms[4]->doorpositiony[1];
          while(y1 != y2-1){
            mvprintw(y1,x1,"#");
            y1++;
        }
        if (x1 != x2){
        if (x1 <x2){
        while(x1!= x2+1){
            mvprintw(y1,x1,"#");
            x1++;
        }}
        else if (x1 > x2)
        {
            while(x1!= x2-1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        }  
        }
        else if (x1==x2) {
            mvprintw(y1,x1, "#");
        }
        mvprintw(y2,x2,"#");
        }  
         
         if (firstroom==4 && secondroom==2){
            int x1=rooms[4]->doorpositionx[1];
         int y1 = rooms[4]->doorpositiony[1]-1;
         int x2 = rooms[1]->doorpositionx[2];
        int y2= rooms[1]->doorpositiony[2];
          while(y1 != y2+1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        if (x1 != x2){
        if (x1 <x2){
        while(x1!= x2+1){
            mvprintw(y1,x1,"#");
            x1++;
        }}
        else if (x1 > x2)
        {
            while(x1!= x2-1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        }  
        }
        else if (x1==x2) {
            mvprintw(y1,x1, "#");
        }
        mvprintw(y2,x2,"#");
        }  
         
        if (firstroom==2 && secondroom==5){
            int x1=rooms[2]->doorpositionx[1];
         int y1 = rooms[2]->doorpositiony[1]+1;
         int x2 = rooms[5]->doorpositionx[1];
        int y2= rooms[5]->doorpositiony[1];
          while(y1 != y2-1){
            mvprintw(y1,x1,"#");
            y1++;
        }
        if (x1 != x2){
        if (x1 <x2){
        while(x1!= x2+1){
            mvprintw(y1,x1,"#");
            x1++;
        }}
        else if (x1 > x2)
        {
            while(x1!= x2-1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        }  
        }
        else if (x1==x2) {
            mvprintw(y1,x1, "#");
        }
        mvprintw(y2,x2,"#");
        }
      
         if (firstroom==5 && secondroom==2){
            int x1=rooms[5]->doorpositionx[1];
         int y1 = rooms[5]->doorpositiony[1]-1;
         int x2 = rooms[2]->doorpositionx[1];
        int y2= rooms[2]->doorpositiony[1];
          while(y1 != y2+1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        if (x1 != x2){
        if (x1 <x2){
        while(x1!= x2+1){
            mvprintw(y1,x1,"#");
            x1++;
        }}
        else if (x1 > x2)
        {
            while(x1!= x2-1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        }  
        }
        else if (x1==x2) {
            mvprintw(y1,x1, "#");
        }
        mvprintw(y2,x2,"#");
        }
       
        if (firstroom==3 && secondroom ==4) {
        int x1=rooms[3]->doorpositionx[0]+1;
        int y1 = rooms[3]->doorpositiony[0];
        int x2 = rooms[4]->doorpositionx[0];
        int y2= rooms[4]->doorpositiony[0];
        while(x1 != x2-1){
            mvprintw(y1,x1,"#");
            x1++;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
         else if (y1==y2){
                        mvprintw(y1,x1,"#");

        }
        
            
        
         mvprintw(y2,x2, "#");
        }
        
        if (firstroom==4 && secondroom ==3) {
        int x1=rooms[4]->doorpositionx[0]-1;
        int y1 = rooms[4]->doorpositiony[0];
        int x2 = rooms[3]->doorpositionx[0];
        int y2= rooms[3]->doorpositiony[0];
        while(x1 != x2+1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
        else if (y1==y2) {
            mvprintw(y1,x1, "#");
        }
            
        
         mvprintw(y2,x2, "#");
        }
        
        if (firstroom==4 && secondroom ==5) {
        int x1=rooms[4]->doorpositionx[2]+1;
        int y1 = rooms[4]->doorpositiony[2];
        int x2 = rooms[5]->doorpositionx[0];
        int y2= rooms[5]->doorpositiony[0];
        while(x1 != x2-1){
            mvprintw(y1,x1,"#");
            x1++;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        
        
        }
        else if (y1==y2){
        mvprintw(y1,x1,"#");

        }
        mvprintw(y2,x2,"#");

        }
       
        if (firstroom==5 && secondroom ==4) {
        int x1=rooms[5]->doorpositionx[0]-1;
        int y1 = rooms[5]->doorpositiony[0];
        int x2 = rooms[4]->doorpositionx[2];
        int y2= rooms[4]->doorpositiony[2];
        while(x1 != x2+1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
        else if (y1==y2) {
            mvprintw(y1,x1, "#");
        }
            
        
         mvprintw(y2,x2, "#");
        
    
            
    }
        
        

   if (numrooms>=7){

        if (firstroom==3 && secondroom==6){
            int x1=rooms[3]->doorpositionx[1];
         int y1 = rooms[3]->doorpositiony[1]+1;
         int x2 = rooms[6]->doorpositionx[1];
        int y2= rooms[6]->doorpositiony[1];
          while(y1 != y2-1){
            mvprintw(y1,x1,"#");
            y1++;
        }
        if (x1 != x2){
        if (x1 <x2){
        while(x1!= x2+1){
            mvprintw(y1,x1,"#");
            x1++;
        }}
        else if (x1 > x2)
        {
            while(x1!= x2-1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        }  
        }
        else if (x1==x2) {
            mvprintw(y1,x1, "#");
        }
        mvprintw(y2,x2,"#");
        }
        
     
         if (firstroom==6 && secondroom==3){
            int x1=rooms[6]->doorpositionx[1];
         int y1 = rooms[6]->doorpositiony[1]-1;
         int x2 = rooms[3]->doorpositionx[1];
        int y2= rooms[3]->doorpositiony[1];
          while(y1 != y2+1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        if (x1 != x2){
        if (x1 <x2){
        while(x1!= x2+1){
            mvprintw(y1,x1,"#");
            x1++;
        }}
        else if (x1 > x2)
        {
            while(x1!= x2-1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        }  
        }
        else if (x1==x2) {
            mvprintw(y1,x1, "#");
        }
        mvprintw(y2,x2,"#");
        
        }
        if (numrooms>=8){
            
        if (firstroom==6 && secondroom ==7) {
        int x1=rooms[6]->doorpositionx[0]+1;
        int y1 = rooms[6]->doorpositiony[0];
        int x2 = rooms[7]->doorpositionx[0];
        int y2= rooms[7]->doorpositiony[0];
        while(x1 != x2-1){
            mvprintw(y1,x1,"#");
            x1++;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        
        }
        else if (y1 == y2){
                        mvprintw(y1,x1, "#");

        }
        mvprintw(y2,x2,"#");

        }
   
        if (firstroom==7 && secondroom ==6) {
        int x1=rooms[7]->doorpositionx[0]-1;
        int y1 = rooms[7]->doorpositiony[0];
        int x2 = rooms[6]->doorpositionx[0];
        int y2= rooms[6]->doorpositiony[0];
        while(x1 != x2+1){
            mvprintw(y1,x1,"#");
            x1--;
        }
        if (y1 != y2){
        if (y1 <y2){
        while(y1!= y2+1){
            mvprintw(y1,x1,"#");
            y1++;
        }}
        else if (y1 > y2)
        {
            while(y1!= y2-1){
            mvprintw(y1,x1,"#");
            y1--;
        }
        }  
        }
        else if (y1==y2) {
            mvprintw(y1,x1, "#");
        }
            
        
         mvprintw(y2,x2, "#");
        }

        }
        }

   attroff(COLOR_PAIR(140));


    }
    return 0;
}

void handleinput(adventurer player1, Room *rooms[], int firstplacen, int numrooms,int stairroom){  
    int ch;
    int ch2;

    while(health>0){
       
        ch = getch();

        int newx= playerx;
        int newy = playery;
        if (ch == 'l' || ch == 'L') {
            newx++;
            if (hunger<250){
                hunger++;
                hungermessage=false;
            }
            else if (hunger>=250){
            hunger=250;     
               if(!hungermessage ){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                    hungermessage=true;
                   
                   }

              if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }
        }
        else if (ch == 'h' || ch == 'H') {newx--;
        
        if (hunger<250){
                hunger++;               
                
         hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
  if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }
        }
        else if (ch == 'k' || ch == 'K') {newy++;if (hunger<250){
                hunger++;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;

  if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }}
        else if (ch == 'j' || ch == 'J') {newy--;if (hunger<250){
                hunger++;
                                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
  if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }}
        else if (ch =='y'|| ch == 'Y') {newx--; newy--;if (hunger<250){
                hunger+=2;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
   if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }}
        else if(ch == 'u' || ch == 'U') {newy--; newx++;if (hunger<250){
                hunger+=2;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
                  if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }}
        else if (ch == 'b'||ch == 'B') {newy++; newx--;if (hunger<250){
                hunger+=2;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
 if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }}
        else if (ch == 'n'||ch == 'N'){newy ++; newx++;if (hunger<250){
                hunger+=2;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
 if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }}
        else if (ch == 'f'|| ch == 'F'){
            ch2 = getch();
            if (ch2 == 'h' || ch2 == 'H'){
                char nextone = mvinch(newy, newx-1) & A_CHARTEXT;

                do{newx--;
                    nextone = mvinch(newy, newx-2) & A_CHARTEXT;}
                 while( nextone == '.' ||  nextone == '#'||  nextone == '+');
                 newx--;
                 if (hunger<250){
                hunger+=4;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
 if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }
                 
            }
            else if (ch2 == 'l' || ch2 == 'L'){
                char nextone = mvinch(newy, newx+1) & A_CHARTEXT;

                do{newx++;
                    nextone = mvinch(newy, newx+2) & A_CHARTEXT;}
                 while( nextone == '.' ||  nextone == '#'||  nextone == '+');
                 newx++;
                 if (hunger<250){
                hunger+=4;               
                 hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
                 if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }
            }
            else if (ch2 == 'k' || ch2 == 'K'){
                char nextone = mvinch(newy+1, newx) & A_CHARTEXT;

                do{newy++;
                    nextone = mvinch(newy+2, newx) & A_CHARTEXT;}
                 while( nextone == '.' ||  nextone == '#'||  nextone == '+');
                 newy++;
                 if (hunger<250){
                hunger+=4;                hungermessage=false;

            }
            else if (hunger>=250){
                hunger=250;
             if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;}
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }
            }
            else if (ch2 == 'j' || ch2 == 'J'){
                char nextone = mvinch(newy-1, newx) & A_CHARTEXT;

                do{newy--;
                    nextone = mvinch(newy-2, newx) & A_CHARTEXT;}
                 while( nextone == '.' ||  nextone == '#'||  nextone == '+');
                 newy--;
                 if (hunger<250){
                hunger+=4;                hungermessage=false;

                
            }
            else if (hunger>=250){
                hunger=250;
                if(!hungermessage){    
                   mvprintw(0,0,"Caution! Your hunger level is at its maximum!");
                   hungermessage=true;
                   }
                if (afterhunger< 40){
                    afterhunger++;
                }
                else if (afterhunger==40){
                    afterhunger=0;
                    health--;

                }
                
            }
            }


        }
        else if (ch=='e'||ch=='E'){
            foodmenu(rooms,numrooms);
        }
        else if (ch=='m' || ch =='M'){
            show_full_map(rooms, numrooms);
        }

        else if (ch =='q' || ch == 'Q') {
            break;
        }
        wint_t nextChar = mvinch(newy, newx) & A_CHARTEXT;
        if (nextChar == '.'){
             start_color();
                init_pair(110,COLOR_MAGENTA,COLOR_BLACK);
                attron(COLOR_PAIR(110));
                mvprintw(playery, playerx, ".");
                attroff(COLOR_PAIR(110));
         
            playerx = newx;
            playery=newy;
            if (strcmp(hero_color,"Green")== 0){
                start_color();
                init_pair(41,COLOR_GREEN,COLOR_BLACK);
                attron(COLOR_PAIR(41));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(41));

            }      
            else if (strcmp(hero_color,"Blue")== 0){
                start_color();
                init_pair(42,COLOR_BLUE,COLOR_BLACK);
                attron(COLOR_PAIR(42));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(42));

            }
            else if (strcmp(hero_color,"Red")== 0){
                start_color();
                init_pair(43,COLOR_RED,COLOR_BLACK);
                attron(COLOR_PAIR(43));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(43));

            }
            else if (strcmp(hero_color,"Yellow")== 0){
                start_color();
                init_pair(44,COLOR_YELLOW,COLOR_BLACK);
                attron(COLOR_PAIR(44));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(44));

            }


        }
        
       else if (nextChar == '+'){
            bool inroom = false;
           
            for(int i = 0; i < numrooms;i++){
                if (playerx > rooms[i]->positionx &&  playerx < rooms[i]->positionx + rooms[i]->width && playery > rooms[i]->positiony && playery < rooms[i]->positiony + rooms[i]->height ){
                    inroom = true;
                    break;
                }
            }

            if(inroom){
                 start_color();
                init_pair(120,COLOR_MAGENTA,COLOR_BLACK);
                attron(COLOR_PAIR(120));
                mvprintw(playery,playerx, ".");
                attroff(COLOR_PAIR(120));
               
            }
            else{
                 start_color();
                init_pair(121,COLOR_YELLOW,COLOR_BLACK);
                attron(COLOR_PAIR(121));
               mvprintw(playery,playerx, "#");
                attroff(COLOR_PAIR(121));
              
            }
            playerx = newx;
            playery=newy;
            if (strcmp(hero_color,"Green")== 0){
                start_color();
                init_pair(41,COLOR_GREEN,COLOR_BLACK);
                attron(COLOR_PAIR(41));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(41));

            }      
            else if (strcmp(hero_color,"Blue")== 0){
                start_color();
                init_pair(42,COLOR_BLUE,COLOR_BLACK);
                attron(COLOR_PAIR(42));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(42));

            }
            else if (strcmp(hero_color,"Red")== 0){
                start_color();
                init_pair(43,COLOR_RED,COLOR_BLACK);
                attron(COLOR_PAIR(43));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(43));

            }
            else if (strcmp(hero_color,"Yellow")== 0){
                start_color();
                init_pair(44,COLOR_YELLOW,COLOR_BLACK);
                attron(COLOR_PAIR(44));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(44));

            }
            
                if(playerx == rooms[0]->doorpositionx[0]&& playery == rooms[0]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][0]){
                        connectrooms(rooms, numrooms, 0, 1);
                        coridor[0][floornumber] = true;
                        coridordrawn1[floornumber][0]=true;
                    }
                }
               else if(playerx == rooms[1]->doorpositionx[0]&& playery == rooms[1]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][0]){
                        connectrooms(rooms, numrooms, 1, 0);
                        coridor[0][floornumber] = true;
                        coridordrawn1[floornumber][0]=true;
                    }
                }
                else if(playerx == rooms[1]->doorpositionx[1]&& playery == rooms[1]->doorpositiony[1]) {
                    if (!coridordrawn1[floornumber][1]){
                        connectrooms(rooms, numrooms, 1,2 );
                        coridor[1][floornumber] = true;
                        coridordrawn1[floornumber][1]=true;
                    }
                }
                 else if(playerx == rooms[2]->doorpositionx[0]&& playery == rooms[2]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][1]){
                        connectrooms(rooms, numrooms, 2,1 );
                        coridor[1][floornumber] = true;
                        coridordrawn1[floornumber][1]=true;
                    }
                }
                else if(playerx == rooms[1]->doorpositionx[2]&& playery == rooms[1]->doorpositiony[2]) {
                    if (!coridordrawn1[floornumber][2]){
                        connectrooms(rooms, numrooms, 2,4 );
                        coridor[2][floornumber] = true;
                        coridordrawn1[floornumber][2]=true;
                    }
                }
                else if(playerx == rooms[4]->doorpositionx[1]&& playery == rooms[4]->doorpositiony[1]) {
                    if (!coridordrawn1[floornumber][2]){
                        connectrooms(rooms, numrooms, 4,2 );
                        coridor[2][floornumber] = true;
                        coridordrawn1[floornumber][2]=true;
                    }
                }
                else if(playerx == rooms[2]->doorpositionx[1]&& playery == rooms[2]->doorpositiony[1]) {
                    if (!coridordrawn1[floornumber][3]){
                        connectrooms(rooms, numrooms, 2,5 );
                        coridor[9][floornumber] = true;
                        coridordrawn1[floornumber][3]=true;
                    }
                }
                else if(playerx == rooms[5]->doorpositionx[1]&& playery == rooms[5]->doorpositiony[1]) {
                    if (!coridordrawn1[floornumber][3]){
                        connectrooms(rooms, numrooms, 5,2 );
                        coridor[9][floornumber] = true;
                        coridordrawn1[floornumber][3]=true;
                    }
                }
                else if(playerx == rooms[3]->doorpositionx[0]&& playery == rooms[3]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][4]){
                        connectrooms(rooms, numrooms, 3,4 );
                        coridor[3][floornumber] = true;
                        coridordrawn1[floornumber][4]=true;
                    }
                }
                else if(playerx == rooms[4]->doorpositionx[0]&& playery == rooms[4]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][4]){
                        connectrooms(rooms, numrooms, 4,3 );
                        coridor[3][floornumber] = true;
                        coridordrawn1[floornumber][4]=true;
                    }
                }
                else if(playerx == rooms[4]->doorpositionx[2]&& playery == rooms[4]->doorpositiony[2]) {
                    if (!coridordrawn1[floornumber][5]){
                        connectrooms(rooms, numrooms, 4,5 );
                        coridor[4][floornumber] = true;
                        coridordrawn1[floornumber][5]=true;
                    }
                }
                else if(playerx == rooms[5]->doorpositionx[0]&& playery == rooms[5]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][5]){
                        connectrooms(rooms, numrooms, 5,4 );
                        coridor[4][floornumber] = true;
                        coridordrawn1[floornumber][5]=true;
                    }
                }
                else if(playerx == rooms[3]->doorpositionx[1]&& playery == rooms[3]->doorpositiony[1]) {
                    if (!coridordrawn1[floornumber][6]){
                        connectrooms(rooms, numrooms, 3,6 );
                        coridor[8][floornumber] = true;
                        coridordrawn1[floornumber][6]=true;
                    }
                }
                else if(playerx == rooms[6]->doorpositionx[1]&& playery == rooms[6]->doorpositiony[1]) {
                    if (!coridordrawn1[floornumber][6]){
                        connectrooms(rooms, numrooms, 6,3 );
                        coridor[8][floornumber] = true;
                        coridordrawn1[floornumber][6]=true;
                    }
                }
                else if(playerx == rooms[6]->doorpositionx[0]&& playery == rooms[6]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][7]){
                        connectrooms(rooms, numrooms, 6,7 );
                        coridor[6][floornumber] = true;
                        coridordrawn1[floornumber][7]=true;
                    }
                }
                else if(playerx == rooms[7]->doorpositionx[0]&& playery == rooms[7]->doorpositiony[0]) {
                    if (!coridordrawn1[floornumber][7]){
                        connectrooms(rooms, numrooms, 7,6 );
                        coridor[6][floornumber] = true;
                        coridordrawn1[floornumber][7]=true;
                    }
                }


            

        }

       else if (nextChar == '#'){
        bool inroom = false;
         for(int i = 0; i < numrooms;i++){
                if (playerx > rooms[i]->positionx &&  playerx < rooms[i]->positionx + rooms[i]->width && playery > rooms[i]->positiony && playery < rooms[i]->positiony + rooms[i]->height ){
                    inroom = true;
                    break;
                }
            }
            if (!inroom){
                start_color();
                init_pair(130,COLOR_YELLOW,COLOR_BLACK);
                attron(COLOR_PAIR(130));
                 mvprintw(playery, playerx, "#");
                attroff(COLOR_PAIR(130));
            }
           
            else {
                 start_color();
                init_pair(131,COLOR_MAGENTA,COLOR_BLACK);
                attron(COLOR_PAIR(131));
                mvprintw(playery, playerx, ".");
                attroff(COLOR_PAIR(131));
              

            }
            playerx = newx;
            playery=newy;
                if (strcmp(hero_color,"Green")== 0){
                start_color();
                init_pair(41,COLOR_GREEN,COLOR_BLACK);
                attron(COLOR_PAIR(41));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(41));

            }      
            else if (strcmp(hero_color,"Blue")== 0){
                start_color();
                init_pair(42,COLOR_BLUE,COLOR_BLACK);
                attron(COLOR_PAIR(42));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(42));

            }
            else if (strcmp(hero_color,"Red")== 0){
                start_color();
                init_pair(43,COLOR_RED,COLOR_BLACK);
                attron(COLOR_PAIR(43));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(43));

            }
            else if (strcmp(hero_color,"Yellow")== 0){
                start_color();
                init_pair(44,COLOR_YELLOW,COLOR_BLACK);
                attron(COLOR_PAIR(44));
                mvprintw(playery,playerx,"@");
                attroff(COLOR_PAIR(44));

            }

        }


    for (int i = 0; i < numrooms; i++) {
        for (int j = 0; j < rooms[i]->foodcount; j++) {
            if ((newx == rooms[i]->foods[j].positionx && newy == rooms[i]->foods[j].positiony)||(newx == rooms[i]->foods[j].positionx +1 && newy == rooms[i]->foods[j].positiony)) {
                int food_value =  1;  
                player.food += food_value;
                start_color();
                init_pair(69,COLOR_YELLOW,COLOR_BLACK);
                attron(COLOR_PAIR(69));
                mvprintw(0, 95, "You collected %d food!", food_value);
                attroff(COLOR_PAIR(69));
                start_color();
                init_pair(130, COLOR_MAGENTA, COLOR_BLACK);
                attron(COLOR_PAIR(130));
                mvprintw(rooms[i]->foods[j].positiony, rooms[i]->foods[j].positionx, ".");  
                mvprintw(rooms[i]->foods[j].positiony, rooms[i]->foods[j].positionx + 1, ".");  
               
                start_color();
                init_pair(120,COLOR_MAGENTA,COLOR_BLACK);
                attron(COLOR_PAIR(120));
                mvprintw(playery,playerx, ".");
                attroff(COLOR_PAIR(120));
                if((newx == rooms[i]->foods[j].positionx && newy == rooms[i]->foods[j].positiony)){
                playerx = rooms[i]->foods[j].positionx;
                playery = rooms[i]->foods[j].positiony;

                }
                else{
                playerx = rooms[i]->foods[j].positionx+1;
                playery = rooms[i]->foods[j].positiony;
                }
                 attroff(COLOR_PAIR(130));


                if (strcmp(hero_color, "Green") == 0) {
                    start_color();
                    init_pair(41, COLOR_GREEN, COLOR_BLACK);
                    attron(COLOR_PAIR(41));
                    mvprintw(playery, playerx, "@");
                    attroff(COLOR_PAIR(41));
                } else if (strcmp(hero_color, "Blue") == 0) {
                    start_color();
                    init_pair(42, COLOR_BLUE, COLOR_BLACK);
                    attron(COLOR_PAIR(42));
                    mvprintw(playery, playerx, "@");
                    attroff(COLOR_PAIR(42));
                } else if (strcmp(hero_color, "Red") == 0) {
                    start_color();
                    init_pair(43, COLOR_RED, COLOR_BLACK);
                    attron(COLOR_PAIR(43));
                    mvprintw(playery, playerx, "@");
                    attroff(COLOR_PAIR(43));
                } else if (strcmp(hero_color, "Yellow") == 0) {
                    start_color();
                    init_pair(44, COLOR_YELLOW, COLOR_BLACK);
                    attron(COLOR_PAIR(44));
                    mvprintw(playery, playerx, "@");
                    attroff(COLOR_PAIR(44));
                }
           rooms[i]->foods[j].positionx = -1;
           rooms[i]->foods[j].positiony = -1;
            } else {
              
                if (wasinroom[floornumber][i] && !rooms[i]->foods[j].collected) {
                    mvprintw(rooms[i]->foods[j].positiony, rooms[i]->foods[j].positionx, "🍕");
                }
            }
        }
    }


        

     for (int i = 0; i < numrooms; i++) {
       for (int j = 0; j < rooms[i]->goldcount; j++) {
        if ((newx == rooms[i]->goldpositionx[j] && newy == rooms[i]->goldpositiony[j])||(newx == rooms[i]->goldpositionx[j]+1 && newy == rooms[i]->goldpositiony[j])) {
            int gold_value = rand() % 10 + 1; 
            player.gold += gold_value;
            mvprintw(0, 0, "You collected %d gold!", gold_value);

      
            start_color();
            init_pair(110, COLOR_MAGENTA, COLOR_BLACK);
            attron(COLOR_PAIR(110));
            mvprintw(rooms[i]->goldpositiony[j], rooms[i]->goldpositionx[j], ".");
            mvprintw(rooms[i]->goldpositiony[j], rooms[i]->goldpositionx[j]+1, ".");
                start_color();
                init_pair(120,COLOR_MAGENTA,COLOR_BLACK);
                attron(COLOR_PAIR(120));
                mvprintw(playery,playerx, ".");
                attroff(COLOR_PAIR(120));
                if(newx == rooms[i]->goldpositionx[j] && newy == rooms[i]->goldpositiony[j]){
            playerx = rooms[i]->goldpositionx[j];
            playery = rooms[i]->goldpositiony[j];
                }
                else{
                playerx = rooms[i]->goldpositionx[j]+1;
            playery = rooms[i]->goldpositiony[j];
                }

            attroff(COLOR_PAIR(110));
        
            if (strcmp(hero_color, "Green") == 0) {
                start_color();
                init_pair(41, COLOR_GREEN, COLOR_BLACK);
                attron(COLOR_PAIR(41));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(41));
            } else if (strcmp(hero_color, "Blue") == 0) {
                start_color();
                init_pair(42, COLOR_BLUE, COLOR_BLACK);
                attron(COLOR_PAIR(42));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(42));
            } else if (strcmp(hero_color, "Red") == 0) {
                start_color();
                init_pair(43, COLOR_RED, COLOR_BLACK);
                attron(COLOR_PAIR(43));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(43));
            } else if (strcmp(hero_color, "Yellow") == 0) {
                start_color();
                init_pair(44, COLOR_YELLOW, COLOR_BLACK);
                attron(COLOR_PAIR(44));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(44));
            }

            rooms[i]->goldpositionx[j] = -1;
            rooms[i]->goldpositiony[j] = -1;
        } else {
            if (wasinroom[floornumber][i]) {
                mvprintw(rooms[i]->goldpositiony[j], rooms[i]->goldpositionx[j], "💵");
            }
        }
    }
 }
 for (int i = 0; i < numrooms; i++){
    for (int j = 0; j < rooms[i]->blackgoldcount; j++) {
        if ((newx == rooms[i]->blackgoldpositionx[j] && newy == rooms[i]->blackgoldpositiony[j])||(newx == rooms[i]->blackgoldpositionx[j]+1 && newy == rooms[i]->blackgoldpositiony[j])) {
            int blackgold_value = rand() % 5 + 50; 
            player.gold += blackgold_value;
            mvprintw(0, 0, "You collected %d gold!", blackgold_value);
            
             start_color();
            init_pair(110, COLOR_MAGENTA, COLOR_BLACK);
            attron(COLOR_PAIR(110));
            mvprintw(rooms[i]->blackgoldpositiony[j], rooms[i]->blackgoldpositionx[j], ".");
            mvprintw(rooms[i]->blackgoldpositiony[j], rooms[i]->blackgoldpositionx[j]+1, ".");
                start_color();
                init_pair(120,COLOR_MAGENTA,COLOR_BLACK);
                attron(COLOR_PAIR(120));
                mvprintw(playery,playerx, ".");
                attroff(COLOR_PAIR(120));
                if(newx == rooms[i]->blackgoldpositionx[j] && newy == rooms[i]->blackgoldpositiony[j]){

             playerx = rooms[i]->blackgoldpositionx[j];
            playery = rooms[i]->blackgoldpositiony[j];
                }
                else{

             playerx = rooms[i]->blackgoldpositionx[j]+1;
            playery = rooms[i]->blackgoldpositiony[j];
                }
            attroff(COLOR_PAIR(110));

            if (strcmp(hero_color, "Green") == 0) {
                start_color();
                init_pair(41, COLOR_GREEN, COLOR_BLACK);
                attron(COLOR_PAIR(41));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(41));
            } else if (strcmp(hero_color, "Blue") == 0) {
                start_color();
                init_pair(42, COLOR_BLUE, COLOR_BLACK);
                attron(COLOR_PAIR(42));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(42));
            } else if (strcmp(hero_color, "Red") == 0) {
                start_color();
                init_pair(43, COLOR_RED, COLOR_BLACK);
                attron(COLOR_PAIR(43));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(43));
            } else if (strcmp(hero_color, "Yellow") == 0) {
                start_color();
                init_pair(44, COLOR_YELLOW, COLOR_BLACK);
                attron(COLOR_PAIR(44));
                mvprintw(playery, playerx, "@");
                attroff(COLOR_PAIR(44));
            }

            rooms[i]->blackgoldpositionx[j] = -1;
            rooms[i]->blackgoldpositiony[j] = -1;

        } else {
        
            if (wasinroom[floornumber][i]) {
                mvprintw(rooms[i]->blackgoldpositiony[j], rooms[i]->blackgoldpositionx[j], "💰");
            }
        }
    }
 } 
     for (int i = 0; i < numrooms; i++) {
    for (int j = 0; j < rooms[i]->weaponcount; j++) {
        char weaponChar = mvinch(rooms[i]->weaponpositiony[j], rooms[i]->weaponpositionx[j]) & A_CHARTEXT;
        if (weaponChar != '@' && wasinroom[floornumber][i] && rooms[i]->weaponTypes[j] == 1) { 
            mvprintw(rooms[i]->weaponpositiony[j], rooms[i]->weaponpositionx[j], "🗡️"); 
        }
    }
 }   
 for (int i = 0; i < numrooms; i++) {
    for (int j = 0; j < rooms[i]->arrowcount; j++) {
        char arrowChar = mvinch(rooms[i]->arrowpositiony[j], rooms[i]->arrowpositionx[j]) & A_CHARTEXT;
        if (arrowChar != '@' && wasinroom[floornumber][i]) {
            mvprintw(rooms[i]->arrowpositiony[j], rooms[i]->arrowpositionx[j], "➳");  
        }
    }
 }
 for (int i = 0; i < numrooms; i++) {
    for (int j = 0; j < rooms[i]->magicwandcount; j++) {
        char magicWandChar = mvinch(rooms[i]->magicwandpositiony[j], rooms[i]->magicwandpositionx[j]) & A_CHARTEXT;
        if (magicWandChar != '@' && wasinroom[floornumber][i]) {
            mvprintw(rooms[i]->magicwandpositiony[j], rooms[i]->magicwandpositionx[j], "!");
        }
    }
 }

        
        
        for (int i =0 ;i < numrooms; i++)
        {
            for (int j =0 ; j< doornumber[i]; j++){
                char doorchar = mvinch (rooms[i]->doorpositiony[j], rooms[i]->doorpositionx[j]) & A_CHARTEXT;
                if (doorchar != '@' && wasinroom[floornumber][i]){
                    mvprintw(rooms[i]->doorpositiony[j], rooms[i]->doorpositionx[j], "+");
                    
                }
                
            }
        }
        
                char stairchar = mvinch (rooms[stairroom]->stairpositiony, rooms[stairroom]->stairpositionx) & A_CHARTEXT;
                if (stairchar != '@' && wasinroom[floornumber][stairroom]&& floornumber<3){
                    mvprintw(rooms[stairroom]->stairpositiony, rooms[stairroom]->stairpositionx, "<");
                }
        if (floornumber == 3) {
          int flag_x = rooms[stairroom]->positionx + rooms[stairroom]->width / 2;
          int flag_y = rooms[stairroom]->positiony + rooms[stairroom]->height / 2;
          if (stairchar != '@' && wasinroom[floornumber][stairroom] && floornumber == 3) {
        mvprintw(flag_y, flag_x, "🚩");
    }

    if ((newx == flag_x && newy == flag_y)||(newx == flag_x+1 && newy == flag_y)) {
    victory_screen(player.gold); 
    main_menu();


    }
   }
                
         for(int i = 0; i < numrooms;i++){
                if (playerx >= rooms[i]->positionx &&  playerx < rooms[i]->positionx + rooms[i]->width && playery >= rooms[i]->positiony && playery <= rooms[i]->positiony + rooms[i]->height ){
                    wasinroom[floornumber][i] = true;
                }
                if (wasinroom[floornumber][i] && !roomdrawn[floornumber][i]){
                    drawroom(rooms[i], i);
                    roomdrawn[floornumber][i]= true;
                }
            }
            if ( newx==rooms[stairroom]->stairpositionx && newy == rooms[stairroom]->stairpositiony){
            clear();
            floornumber++;
            start();
        }
           show_attributes(); 
           if(health == 0){
              game_over_screen();
              main_menu();
              
           }          
        }

    }
void show_attributes() {
    int x_position = COLS - 40; 
    int y_position = 0;
    int health_bar_length = 20; 
    int filled_length = (health / maxhealth) * health_bar_length;

    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    init_pair(2, COLOR_RED, COLOR_BLACK);  

    move(y_position, x_position); clrtoeol();  
    move(y_position + 1, x_position); clrtoeol();  

    mvprintw(y_position, x_position-1, "Health: [");

    attron(COLOR_PAIR(1));
    for (int i = 0; i < filled_length; i++) {
        printw("|");
    }
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for (int i = filled_length; i < health_bar_length; i++) {
        printw(".");
    }
    attroff(COLOR_PAIR(2));


    printw("] %.1f/%.1f", health, maxhealth);


    mvprintw(y_position + 1, x_position-1, "Hunger: %.1f", hunger);
    mvprintw(y_position + 2, x_position-1, "Gold: %d", player.gold);
    mvprintw(y_position + 3, x_position-1, "Food: %d", player.food);

    refresh();
}
void show_full_map(Room *rooms[], int num_rooms) {
    clear(); 
  
    mvprintw(50, 100, "Floor: %d", floornumber+1);
    for (int i = 0; i < num_rooms; i++) {
        
        drawroom(rooms[i], i);  
        start_color();
        init_pair(1, COLOR_YELLOW,COLOR_BLACK);
        attron(COLOR_PAIR(1));
        connectrooms(rooms, num_rooms, 0, 1);
        connectrooms(rooms, num_rooms, 1, 2);
        connectrooms(rooms, num_rooms, 2, 3);
        connectrooms(rooms, num_rooms, 3, 4);
        connectrooms(rooms, num_rooms, 4, 5);      
        connectrooms(rooms, num_rooms, 5, 6);
        connectrooms(rooms, num_rooms, 6, 7);
        connectrooms(rooms, num_rooms, 3,6 );
        connectrooms(rooms, num_rooms, 2,5 );
        connectrooms(rooms, num_rooms, 2,4 );
        attroff(COLOR_PAIR(1));
       
    }

    int ch3;
    ch3 = getch();
    if (ch3 == 'm' || ch3 == 'M') {
        clear();
       
        mvprintw(50, 100, "Floor: %d", floornumber+1);
        for (int i = 0; i < num_rooms; i++) {
            if (roomdrawn[floornumber][i]) {
                drawroom(rooms[i], i); 
                
            }
            start_color();
            init_pair(90,COLOR_YELLOW,COLOR_BLACK);
            attron(COLOR_PAIR(90));
            if (coridor[0][floornumber])
            connectrooms(rooms, num_rooms, 0, 1);
            if ( coridor[1][floornumber])
            connectrooms(rooms, num_rooms, 1, 2);
            if (coridor[2][floornumber])
            connectrooms(rooms, num_rooms, 2,4 );
            if(coridor[3][floornumber])
            connectrooms(rooms, num_rooms, 3, 4);
            if(coridor[4][floornumber])
            connectrooms(rooms, num_rooms, 4,5 );
            if(coridor[6][floornumber])
            connectrooms(rooms, num_rooms, 6,7 );
            if(coridor[9][floornumber])
            connectrooms(rooms, num_rooms, 2,5 );
            if(coridor[8][floornumber])
            connectrooms(rooms, num_rooms, 3,6 );
            attroff(COLOR_PAIR(90));


        }
    }

 }
void change_music() {
    int choice;
    WINDOW *music_win;
    int height = 10, width = 50;
    int starty = 25, startx = 85;
    char *music_choices[] = {
        "Music 1",
        "Music 2",
        "Music 3",
        NULL, 
        "Back"
    };

    music_win = newwin(height, width, starty, startx);
    keypad(music_win, TRUE);
    refresh();
    
    int highlight = 1;

    while (1) {
        if (music_enabled) {
            music_choices[3] = "Turn Off Music";
        } else {
            music_choices[3] = "Turn On  Music";
        }

        wattron(music_win, COLOR_PAIR(1));
        wborder(music_win, '#', '#', '#', '#', '#', '#', '#', '#');
        wattroff(music_win, COLOR_PAIR(1));
        print_menu(music_win, highlight, music_choices, 5); 
        
        int c = wgetch(music_win);
        if (c == KEY_UP) {
            if (highlight == 1) {
                highlight = 5; 
            } else {
                --highlight;
            }
        } else if (c == KEY_DOWN) {
            if (highlight == 5) {
                highlight = 1; 
            } else {
                ++highlight;
            }
        } else if (c == 10) { 
            choice = highlight;
            if (choice == 5) {
                break;
            }

            if (choice == 1) {
                strcpy(selected_music, "music1.mp3");
                music_enabled = 1; 
            } else if (choice == 2) {
                strcpy(selected_music, "music2.mp3");  
                music_enabled = 1;  
            } else if (choice == 3) {
                strcpy(selected_music, "music3.mp3");  
                music_enabled = 1; 
            } else if (choice == 4) {
                if(music_enabled == 1)
                music_enabled=0;
            else{
                music_enabled = 1;
            }
            
            }
        }

        clear();
        refresh();
    }
}
void foodmenu(Room* rooms[], int num_rooms) {
    clear();
    mvprintw(25, 85, "Press a letter to choose an action:");
    mvprintw(27, 85, "a ) Eat food (%d portions available)", player.food);
    mvprintw(29, 85, "q ) Exit menu");
    refresh();

    while (1) {
        int ch = getch();

        if (ch == 'a' || ch == 'A') {
            if (player.food > 0) {
                player.food--; 
                mvprintw(40, 95, "Food consumed: 1 portion!");
    
                if (health < maxhealth) {
                    health += 1;
                }
                hunger = (hunger > 100) ? hunger - 100 : 0;
                mvprintw(27, 85, "a ) Eat food (%d portions available)  ", player.food);
                refresh();
            } else {
      
             mvprintw(40, 95, "No food left!");
               
               
                refresh();
            }
        } 
        else if (ch == 'q' || ch == 'Q') {
            break; 
        }
    }

    clear();
    for (int i = 0; i < num_rooms; i++) {
        if (roomdrawn[floornumber][i]) {
            drawroom(rooms[i], i);
        }
    }

    start_color();
    init_pair(90, COLOR_YELLOW, COLOR_BLACK);
    attron(COLOR_PAIR(90));

    if (coridor[0][floornumber]) connectrooms(rooms, num_rooms, 0, 1);
    if (coridor[1][floornumber]) connectrooms(rooms, num_rooms, 1, 2);
    if (coridor[2][floornumber]) connectrooms(rooms, num_rooms, 2, 4);
    if (coridor[3][floornumber]) connectrooms(rooms, num_rooms, 3, 4);
    if (coridor[4][floornumber]) connectrooms(rooms, num_rooms, 4, 5);
    if (coridor[6][floornumber]) connectrooms(rooms, num_rooms, 6, 7);
    if (coridor[9][floornumber]) connectrooms(rooms, num_rooms, 2, 5);
    if (coridor[8][floornumber]) connectrooms(rooms, num_rooms, 3, 6);
    
    attroff(COLOR_PAIR(90));

    show_attributes(); 
    mvprintw(50, 100, "Floor: %d", floornumber + 1);
    refresh(); 
}
void show_temporary_message(int y, int x,const char *message) {
    
    char backup[256];
    mvinnstr(y, x, backup, sizeof(backup) - 1);
    mvprintw(y, x, message);
    refresh();
    int ch;
    do {
        ch = getch();
    }
    while(ch != ' ');
    
    if (ch==' '){
    mvprintw(y, x, backup);
    refresh();
    }
    return;
}
void game_over_screen() {
    clear();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK); 
    attron(COLOR_PAIR(1)); 

    int row = LINES / 2 - 3; 
    int col = (COLS - 35) / 2;
    mvprintw(row, col - 15, "   ██████   █████  ███    ███ ███████     ███████  ██    ██ ███████ ██████  ");
    mvprintw(row + 1, col - 15, " ██       ██   ██ ████  ████ ██          ██   ██  ██    ██ ██      ██   ██ ");
    mvprintw(row + 2, col - 15, " ██   ███ ███████ ██ ████ ██ █████       ██   ██  ██    ██ █████   ██████  ");
    mvprintw(row + 3, col - 15, " ██    ██ ██   ██ ██  ██  ██ ██          ██   ██  ██    ██ ██      ██   ██ ");
    mvprintw(row + 4, col - 15, "  ██████  ██   ██ ██      ██ ███████     ███████  ████████ ███████ ██   ██ ");
    
    attroff(COLOR_PAIR(1));
    refresh();

    napms(3000);
    endwin();   
}
void victory_screen(int total_gold) {
    clear();
    start_color();
    init_pair(1, COLOR_GREEN, COLOR_BLACK); 
    attron(COLOR_PAIR(1)); 

    int row = LINES / 2 - 3; 
    int col = (COLS - 35) / 2; 

    mvprintw(row, col - 15, "  V   V   III  CCCCC  TTTTT   OOOOO   RRRR    Y   Y  ");
    mvprintw(row + 1, col - 15, "  V   V    I   C        T    O     O  R   R    Y Y   ");
    mvprintw(row + 2, col - 15, "  V   V    I   C        T    O     O  RRRR      Y    ");
    mvprintw(row + 3, col - 15, "  V   V    I   C        T    O     O  R  R      Y    ");
    mvprintw(row + 4, col - 15, "   VVV    III  CCCCC    T     OOOOO   R   R     Y    ");
    attroff(COLOR_PAIR(1)); 
    mvprintw(row + 10, col+3 , "Total Gold: %d", player.gold);

    refresh();

    napms(3000);  
    
    main_menu();
}

int main() {
    setlocale(LC_ALL, "");
    main_menu();
    return 0;
}