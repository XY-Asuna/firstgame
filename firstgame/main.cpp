#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
int main()
{
    int rx,ry;
    // create the window
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "My window");
    sf::Font font; // Throws sf::Exception if an error occurs
    font.openFromFile("ziti/SourceHanSerifCN-Medium.otf");
    sf::Text text(font); // a font is required to make a text object

    // set the string to display
    text.setString(L"ÄãºÃÊÀ½çhelloworld");

    // set the character size
    text.setCharacterSize(24); // in pixels, not points!

    // set the color
    text.setFillColor(sf::Color::Red);

    // set the text style
    text.setStyle(sf::Text::Bold | sf::Text::Underlined);


        // inside the main loop, between window.clear() and window.display()
    
    //text.setPosition(sf::Vector2);
    // run the program as long as the window is open
    while (window.isOpen())
    {
        srand((unsigned)time(NULL));
        rx = rand() % 800;
        //srand((unsigned)time(NULL));
        ry = rand() % 600;
        text.setPosition(sf::Vector2f(rx, ry));
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        window.clear(sf::Color::White);
        // clear the window with black color
        //window.clear(sf::Color::Black);
        window.draw(text);
        // draw everything here...
        // window.draw(...);

        // end the current frame
        window.display();
    }
}