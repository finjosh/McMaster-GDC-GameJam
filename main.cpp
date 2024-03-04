#include <iostream>

#include "SFML/Graphics.hpp"

#include "TGUI/TGUI.hpp"
#include "TGUI/Backend/SFML-Graphics.hpp"

#include "box2d/Box2D.h"

#include "Utils/Utils.hpp"
#include "Utils/Graphics/WindowHandler.hpp"
#include "Utils/Physics/WorldHandler.hpp"

#include "Utils/UpdateManager.hpp"
#include "Utils/Graphics/DrawableManager.hpp"
#include "Utils/ObjectManager.hpp"
#include "Utils/Physics/CollisionManager.hpp"

#include "Player.hpp"
#include "Enemy.hpp"

using namespace std;
using namespace sf;

void addThemeCommands();

// class test : public virtual Object, public DrawableObject
// {
// public:
//     void Draw(sf::RenderWindow& window) override
//     {
//         sf::RectangleShape temp({20,20});
//         temp.setOrigin(10,10);
//         temp.setPosition({this->getPosition().x*PIXELS_PER_METER, this->getPosition().y*PIXELS_PER_METER});
//         temp.setRotation(this->getRotation()*180/b2_pi);
//         temp.setOutlineColor(sf::Color::Black);
//         temp.setOutlineThickness(1);
//         window.draw(temp);
//     }

//     createDestroy();
// };

// TODO setup a view manager that handles windows size changes
int main()
{
    // setup for sfml and tgui
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "Example Game" /*, sf::Style::Fullscreen*/);
    window.setFramerateLimit(60);
    WindowHandler::setRenderWindow(&window);

    tgui::Gui gui{window};
    gui.setRelativeView({0, 0, 1920/(float)window.getSize().x, 1080/(float)window.getSize().y});
    tgui::Theme::setDefault("themes/Dark.txt");
    Command::color::setDefaultColor({255,255,255,255});
    // -----------------------

    WorldHandler::getWorld().SetGravity({0.f,0.f});
    WorldHandler::getWorld().SetContactListener(new CollisionManager);

    //! Required to initialize VarDisplay and CommandPrompt
    // creates the UI for the VarDisplay
    VarDisplay::init(gui); 
    // creates the UI for the CommandPrompt
    Command::Prompt::init(gui);
    addThemeCommands();
    // create the UI for the TFuncDisplay
    TFuncDisplay::init(gui);

    //! ---------------------------------------------------

    auto temp = new Enemy(25,25, {new Player(10,10)});
    temp->destroy();

    sf::View camera(sf::FloatRect{0,0,1920,1080});
    window.setView(camera);

    UpdateManager::Start();
    sf::Clock deltaClock;
    float fixedUpdate = 0;
    while (window.isOpen())
    {
        EventHelper::Event::ThreadSafe::update();
        window.clear();
        // updating the delta time var
        sf::Time deltaTime = deltaClock.restart();
        fixedUpdate += deltaTime.asSeconds();
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();

            gui.handleEvent(event);

            //! Required for LiveVar and CommandPrompt to work as intended
            LiveVar::UpdateLiveVars(event);
            Command::Prompt::UpdateEvent(event);
            //! ----------------------------------------------------------
        }
        UpdateManager::Update(deltaTime.asSeconds());
        if (fixedUpdate >= 0.2)
        {
            fixedUpdate = 0;
            UpdateManager::FixedUpdate();
        }
        UpdateManager::LateUpdate(deltaTime.asSeconds());
        //! Updates all the vars being displayed
        VarDisplay::Update();
        //! ------------------------------=-----
        //! Updates all Terminating Functions
        TerminatingFunction::UpdateFunctions(deltaTime.asSeconds());
        //* Updates for the terminating functions display
        TFuncDisplay::update();
        //! ------------------------------

        //! Do physics before this
        WorldHandler::updateWorld(deltaTime.asSeconds());
        CollisionManager::Update();
        //! Draw after this

        //* Write code here

        // ---------------

        DrawableManager::draw(window);

        // draw for tgui
        gui.draw();
        // display for sfml window
        window.display();
    }

    //! Required so that VarDisplay and CommandPrompt release all data
    VarDisplay::close();
    Command::Prompt::close();
    TFuncDisplay::close();
    //! --------------------------------------------------------------

    window.close();

    return 0;
}

void addThemeCommands()
{
    Command::Handler::addCommand(Command::command{"setTheme", "Function used to set the theme of the UI (The previous outputs in the command prompt will not get updated color)", 
        {Command::print, "Trying calling one of the sub commands"},
        std::list<Command::command>{
            Command::command{"default", "(Currently does not work, coming soon) Sets the theme back to default", {[](){ 
                tgui::Theme::setDefault(""); //! This does not work due to a tgui issue
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}},
            // Dark theme is a custom theme made by me 
            // It can be found here: https://github.com/finjosh/TGUI-DarkTheme
            Command::command{"dark", "Sets the them to the dark theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Dark.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}}, 
            Command::command{"black", "Sets the them to the black theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/Black.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({255,255,255,255}); // white
            }}},
            Command::command{"grey", "Sets the them to the transparent grey theme", {[](){ 
                tgui::Theme::getDefault()->load("themes/TransparentGrey.txt"); 
                // Note that command color does not update with theme so you have to set the default color
                Command::color::setDefaultColor({0,0,0,255}); // black
            }}}
        }
    });
}