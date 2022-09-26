#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <array>
#include <ctime>
#include <cstdlib>

using namespace sf;

/// Variables needed for creation of the window
RenderWindow window;
const int WINDOW_W = 1024;
const int WINDOW_H = 1024;
float FPS = 20;


/// Variables used in creation of the cells
const int res = 14;
const int BOX_W = WINDOW_W / res;
const int BOX_H = WINDOW_H / res;
struct cell {

    RectangleShape shape;
    bool alive;

    // What are you even gonna do with this? -- Make em green lol
    int ticksAlive = 0;

    void incTicks(){
        if(ticksAlive < 255)
            ticksAlive+=3;
    }

};

// This is the 'Game of Life Grid' - Where all out little cubes will live
std::array<std::array<cell, BOX_W>, BOX_H> grid;

/// Flips the cell to the opposite color (White --> Black || Black --> White)
void flipColor(cell& box){

    if(box.alive){
        box.shape.setFillColor(Color(255,255,255));
        box.alive = false;
        box.ticksAlive = 0;
    }
    else {
        box.shape.setFillColor(Color(0,box.ticksAlive,0));
        box.alive = true;
        box.incTicks();
    }
}

/// Takes the sum of the eight cells around a given cell
int sumNeighbors(int row, int col){

/*
 * These rules, which compare the behavior of the automaton to real life, can be condensed into the following:
        Any live cell with two or three live neighbours survives.
        Any dead cell with three live neighbours becomes a live cell.
        All other live cells die in the next generation. Similarly, all other dead cells stay dead.
 */

    int sum = 0;

    if(grid[row][col].alive){
        sum-=1;
    }

    for(int x = -1;x < 2;x++){
        for(int y = -1;y < 2;y++){
            // Sum up the surrounding cells that are alive
            if(grid[row+x][col+y].alive){
                sum+=1;
            }
        }
    }

    return sum;
}

///Uses the current state of the grid to determine who lives and who dies
std::array<std::array<cell, BOX_W>, BOX_H> letThereBeLife(){

    std::array<std::array<cell, BOX_W>, BOX_H> grid_next;

    for(int row = 0; row < grid.size();row++){
        for(int col = 0; col < grid.size(); col++){
            cell currentBox = grid[row][col];

            int sum = sumNeighbors(row, col);

            if(currentBox.alive){
                if(sum < 2 || sum > 3){
                    // This cell is now dead
                    flipColor(currentBox);
                    currentBox.ticksAlive = 0;
                }
                else{
                    currentBox.incTicks();
                    currentBox.shape.setFillColor(Color(0,currentBox.ticksAlive,0));
                }
            }
            else if(!currentBox.alive && sum == 3){
                // This cell is now alive
                flipColor(currentBox);
                currentBox.incTicks();
                currentBox.shape.setFillColor(Color(0,currentBox.ticksAlive,0));
            }

            grid_next[row][col] = currentBox;
        }
    }

    return grid_next;
}

/// Set a single cell with a color and a state (LOOK IN STRUCT CELL FOR MORE)
void initShape(cell& box){

    box.shape.setSize(Vector2f(res, res));

    int choice = rand() % 3;

    if(choice == 1){
        box.shape.setFillColor(Color(0,box.ticksAlive,0));
        box.alive = true;
        box.incTicks();
    }
    else {
        box.shape.setFillColor(Color::White);
        box.alive = false;
        box.ticksAlive = 0;
    }

}

/// Fills the 2D grid --> This is also used for resetting while running (Press the 'r' key)
void initGrid(){

    float posx = 0;
    float posy = 0;

    for(auto& row : grid){
        for(auto& col : row){
            cell box;
            initShape(box);
            box.shape.setPosition(posx*res, posy*res);
            col = box;
            posy++;
        }
        posy = 0;
        posx++;
    }


}

/// Loops Through 2D grid and draws each cell
void drawGrid(){

    for(auto& row : grid){
        for(auto& col : row){
            window.draw(col.shape);
        }
    }
}

/// Draws the current grid and stores the next values into 'grid'
void draw() {
    drawGrid();
    grid = letThereBeLife();
    window.display();
}

/// MAIN LOOP - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int main() {

    // Allow for random generation of the cells and their colors
    srand (time(0));

    // Create a basic window
    window.create(VideoMode(WINDOW_W, WINDOW_H), "Game Of Life");
    window.setFramerateLimit(FPS);

    // Fill up the grid array with Rectangle Objects
    initGrid();

    // This is the main window loop
    while (window.isOpen())
    {
        if(FPS != 0)
            draw();

        // check all the window's events that were triggered since the last iteration of the loop
        Event event{};
        while (window.pollEvent(event))
        {

            // "close requested" event: we close the window
            if (event.type == Event::Closed)
                window.close();

            // Speed up and slow down sim - - - - - - - - - - - - - - - - -
            if(event.key.code == Keyboard::Up || event.mouseWheelScroll.delta == 1){
                FPS+=2.5;
                window.setFramerateLimit(FPS);
            }
            if(event.key.code == Keyboard::Down || event.mouseWheelScroll.delta == -1){
                if(FPS > 0)
                    FPS-=2.5;
                window.setFramerateLimit(FPS);

            }
            // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

            // Reset Grid -- Press the 'r' key || Right click mouse
            if(event.key.code == Keyboard::R || event.mouseButton.button == 1){
                initGrid();
            }
        }
    }

    return 0;
}
