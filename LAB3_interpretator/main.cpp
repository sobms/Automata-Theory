#include <SFML/Graphics.hpp>
#include "StreeNodes.h"
#include <fstream>
#include <sstream>

using namespace sf;
Program_data pdata = Program_data();



int main () {
    std::ifstream file;
    file.open("robot_path.txt");
    int flag = false;

    RenderWindow window(sf::VideoMode(pdata.robot.get_side()*64, pdata.robot.get_side()*64), "LABIRINT");

    Image wall;
    wall.loadFromFile("pictures/wall.png");
    Texture wall_texture;
    wall_texture.loadFromImage(wall);
    Sprite wall_spr;
    wall_spr.setTexture(wall_texture);

    Image empty;
    empty.loadFromFile("pictures/empty.png");
    Texture empty_texture;
    empty_texture.loadFromImage(empty);
    Sprite empty_spr;
    empty_spr.setTexture(empty_texture);

    Image exit;
    exit.loadFromFile("pictures/exit.png");
    Texture exit_texture;
    exit_texture.loadFromImage(exit);
    Sprite exit_spr;
    exit_spr.setTexture(exit_texture);


    Image robot_img;
    robot_img.loadFromFile("pictures/robot1.png");
    Texture robot_texture;
    robot_texture.loadFromImage(robot_img);
    Sprite robot_sprite;
    robot_sprite.setTexture(robot_texture);
    int pos_x, pos_y;
    std::string line, x, y;

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                    window.close();
        }

        //window.clear();
        //window.draw(shape);
        for (int x = 0; x < pdata.actual_map.get_side(); x++) {
            for (int y = 0; y < pdata.actual_map.get_side(); y++) {
                if (pdata.actual_map.get_cell(x,y) == cellval::EMPTY) {
                    empty_spr.setPosition(x * 64, y * 64);
                    window.draw(empty_spr);
                }
                if (pdata.actual_map.get_cell(x,y) == cellval::EXIT) {
                    exit_spr.setPosition(x * 64, y * 64);
                    window.draw(exit_spr);
                }
                if (pdata.actual_map.get_cell(x,y) == cellval::WALL) {
                    wall_spr.setPosition(x * 64, y * 64);
                    window.draw(wall_spr);
                }
                
            }
        }
        

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && !flag && !file.eof()) {
            
            getline(file, line);
            std::istringstream f(line);   
            getline(f, x, ',');
            getline(f, y);
            std::cout << x << " " << y << std::endl;
            if ((x!= "") && (y != "")) {
                pos_x = atoi(x.c_str()) * 64;
                pos_y = atoi(y.c_str()) * 64;
            }
            flag = true;
        }
        else if (!sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
            flag = false;
        }
        robot_sprite.setPosition(pos_x, pos_y);
        window.draw(robot_sprite);
        window.display();
    } 
    file.close();
}