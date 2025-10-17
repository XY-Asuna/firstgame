#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <thread>
#include <memory>
#include <iostream>
#include <mutex>
#include <sstream>
#include <optional>
#include <stdio.h>
#include <immintrin.h>
#include <chrono>
sf::RenderWindow window;
std::optional<sf::Event> start_event;
std::mutex mtx,grade_mtx;//lock
float lac = 1,bomb_speed;
int totlen,totlen_copy,grade,state,totbomb;
sf::CircleShape circle(20.f),snake[100006],apple,bomb[1000];
sf::CircleShape circle_copy(20.f), snake_copy[100006],apple_copy;
sf::Time frametime = sf::microseconds(1000),frametime_rend=sf::microseconds(16667),bomb_time_set=sf::seconds(10);
sf::Clock frameclock,frameclock_rend,bomb_time;
sf::RectangleShape boundary({ 780.f,580.f });
/*// sf::Font font;
// sf::Text text(font);//  ��Ҫ���ӳٳ�ʼ���ġ���
//������� �ӳٵ� ��������font ������font�������� ��֮ǰһֱ��ôд��û����
//��Ȼ������ ������ʵ���������� ���Ǻ�ϰ�� 
// ������ǰ��Щȫ�ֱ�����ʼ��˳��ûָ��������
//ohno ����text������������ж��� ��û�취������font��*/
std::unique_ptr<sf::Font> font{};
std::unique_ptr<sf::Text> text{}, text_grade{}, start_text{}, end_text{};
/*// ɵ��vs ���ز�������ֱ�Ӳ�ȫ���߰���Ķ���

//fking����û���ù� û������
//ptr��ʲô���� pointer
//��Ҫ�����Կ��ܲ�����*/
void drawing_ing()
{
    window.draw(boundary);
    window.draw(*text);
    window.draw(apple_copy);
    for (int i = 1; i <= totlen_copy; i++) window.draw(snake_copy[i]);
    window.draw(circle_copy);
    window.draw(*text_grade);
    for(int i=1;i<=totbomb;i++) window.draw(bomb[i]);
}
void darwing_start()
{
    window.draw(*start_text);
}
void darwing_end()
{
    window.draw(*end_text);
}
void Copy()
{
    totlen_copy = totlen;
    for (int i = 1; i <= totlen_copy; i++) snake_copy[i] = snake[i];
    apple_copy = apple;
    circle_copy = circle;
}
void renderingThread(sf::RenderWindow* window)
{
    // activate the window's context
    window->setActive(true);
    // the rendering loop
    while (window->isOpen())
    {
        // draw...
        if (frametime_rend.asMilliseconds() > frameclock_rend.getElapsedTime().asMilliseconds()) continue;
        frameclock_rend.restart();
        mtx.lock();
        Copy();
        mtx.unlock();
        window->clear(sf::Color::White);
        if(state==0) darwing_start();
        if (state == 1)
        {
            mtx.lock();
            Copy();
            mtx.unlock();
            drawing_ing();
        }
        
        if(state==2) darwing_end();
        window->display();
    }
}/*
// ���㿴�� unique pointer��ôʵ��
// �Ȼ���㽲��
//ohno����ʲô ������ǿ�������

// �����ģ��
// Ҳ����˵ ��������в��� ������������ͱ���
// ��������vector ����������д�ľ���ģ�����Ͳ��� eg vector<int> a;
// Ȼ����ͷ���vector�������Ӧ���������� �ȿ���vector<int> Ҳ����vector<char> ȷʵ
// ����ģ������������
// my_unique_ptr<int> => T = int
template<typename T>
struct my_unique_ptr {
    my_unique_ptr() = default; // Ĭ�Ϲ��� ������ �õ�
    T* storage{};               // �������ÿ��� //�����û�õĳ�ʼ�� ������

    template<typename... Args> // ����������ǿɱ����������ʾһ������ //����һЩint char ֮��� ������һ��
    my_unique_ptr(Args&&... args) { // Ȼ��������캯�����ܿɱ�Ĳ��� ...����չ������� ���������÷�������Ϊ�������۵� Ȼ���������������
        // �����۵�Ҳ����˵ T && && = T && T & && = T & 
        // ���������ÿ����Ƶ����������õ����� ��������������þ����Ƶ����������õ�������һ��
        // ���� int & �ͽа������õ����� int ����û�����õ� //����Ҵ��֪��
        storage = new T(std::forward<Args>(args)...);// ... ��չ��ǰ��������ʽ
        // �������Ǵﵽ��Ŀ�� ����  �� new T(...) ���Ƿ��䲢����һ�� T ����һ�� T* ��ȷ�ޱ�
    }

    // ©���Ǻ������
    T &operator*() const noexcept {
        return *storage;
    }
    // ��©�˼�ͷ
    T *operator->() const noexcept {
        return storage;
    } // sb vs ����û��

    // ���紫��(9, std::string(), str), str ��һ�� std::string ����
    // ��ô Args ���Ƶ��� (int(�ð�����������������������ʲô �������int//��int�� С���� int &&), std::string &&, std::string &)
    // 
    // template<typename T> func(T t) {...}  func(int) => T = int ���粻���Զ��Ƶ� ��Ҫ�ֶ�д ����make_unique<T> ���������fuck<T>(...)
    // my_unique_ptr(int, float) => Args = [int, float] �������
    // �����ģ�庯��
    // �и������� sort
    // ����һ������ط���sort�����ںܶණ����
    // sort(����),vector,ʲôʲô�� 
    // �����Զ��Ƶ�
    // ��Ҳ������ģ�� ����д����������Ϊͨ�������Զ��Ƶ���ģ�����Ͳ����� //�ĸ��������Զ��Ƶ���

    my_unique_ptr(const my_unique_ptr &) = delete;
    my_unique_ptr& operator=(const my_unique_ptr&) = delete;
    // ��һ���и��ƹ��캯�� ��Ȼ unique ptr ���ܸ��� ��Ϊ�����������Ȩ
    // c++ ����Щ��Ҫ����������ù�����Ķ���
    // rust ������Ȩ��Ȼ���ϸ� c++ ��Ҫ���Լ��ֶ�����Щ��
    // �����Ǹ������������ ���ƹ��츳ֵ����� ���԰��������� delete �� �������������
    // ���㳢�Ը���unique_ptr �ͻᱨ�� ������
    //���ǰ�û�õ�delete
    
    my_unique_ptr(my_unique_ptr &&other) noexcept : storage(other.storage) {
        other.storage = nullptr;
    }
    my_unique_ptr& operator=(my_unique_ptr &&other) noexcept {
        storage = other.storage;
		other.storage = nullptr;
        return *this;
	}
    // ���ܸ��� ����ô���� unique_ptr ��
    // c++ �и����ƶ��Ķ�������ȻrustҲ�� ������������һ��
    // rust �ﱻ�ƶ��Ķ�������Ѿ����ݻٵ�״̬ // �ƶ���ݻ�ԭ����?//�� �������������������Ų���� ԭ��
    // c++ �ƶ�֮�� ԭ���������û�С���ʧ�����ߴݻ� ��˵���������� ����һ��Ҫ���ڿ��԰�ȫ������״̬
    // �������������ƶ����캯�� �Ͷ�Ӧ�ĸ�ֵ���� //�Ѹ�ֵ��=�������������ı�
    // ��������ֵ�Ż��������ƶ� ������
    // ���Կ������ǰ����ָ��ת�Ƶ��µ� unique_ptr ���� �ɵ�Ҫ�ÿ� �������ڿɰ�ȫ������״̬
    
    //���� == �ݻ٣�
    // ��˵��Ҳ��
    // ����������� �������߳��������� scope ֮�󣬱������������� //okok
    // ��Ȼ�������ܷܺ���ʵ��һЩ�����߼�
    // ������ unique_ptr ��Ȼ����Ҫ�ͷ�������� ����ֱ�� delete delete ��ָ��û�� ���Բ�����
    // ����� RAII resource acquisition is initialization, �����ʱ����ɳ�ʼ�� Ȼ���Զ��� scope ���� �ͷ��������Դ
    // ��ȻҲ�������÷� �����и�std::lock_guard ����һ��mutex��������� �����ʱ������ ������ʱ�����
    ~my_unique_ptr() {
        delete storage;
    }
    //Ҳ���� unique_ptr �����Զ��Ƶ����� ������Զ��ͷ�
};

template<typename T, typename... Args>
auto fuck(Args&&... args) { // ��������Ϊ������Args�Ƶ��������õ�����
    return T(std::forward<Args>(args)...);//���forward��ʲô���� 
    // Ȼ�����Forwardʹ�ô��������� �� args ��ԭ�������ͣ�Args������//�ܴ���
    //Ҳ����˵ �Ҹ�fuckһ������ �ͻ᷵�� T(�������)
}
// ��Ҫ��ֵ�����й�ϵ �����Ƚ���� һ��ת��������������� ��֤��fuck<T>(...) T(...), ���ܵ�ֵ��һ���� ԭ������ֵ���Ǵ�����ֵ ԭ������ֵ�ʹ�����ֵ ���õ���ȷ�ĺ���
//okok
// 
// arguments ���ǲ���
//arg is what
*/
void setup_ur_fking_text() {
   /* //������ָ�벻���á�
    // ̫������le std::unique_ptr
    //����ָ����ô����
    //û��������ָ��
    //��������ܵ���˼�� �����ں��ʵ�ʱ���ͷŵ�����Ķ��� ���Բ����ֶ�delete
    // sf::Font(const std::filesystem::path &path)//�̶��Ĳ��� ���������Զ�ת�� again
    // ȷʵ make_unique<T>(args) ����ת�������� T(args)
    //������ sf::Font font("dsaasdasd.otf");*/
    font = std::make_unique<sf::Font>(std::filesystem::path("SourceHanSerifCN-Medium.otf"));//�����ǹ�����һ��Font���͵Ķ��� 
    //��������ô֪�����õ���font.setFontfile
    //sf::Font f = fuck<sf::Font>(std::filesystem::path("SourceHanSerifCN-Medium.otf")); // �ǳ����� �����ԭ��
    text = std::make_unique<sf::Text>(*font);
    
    /*//auto test1 = std::move(text); // std::move �Ѳ���������ת��Ϊ��ֵ���÷���
    //auto test2(std::move(test1)); // ����͵������ƶ����캯��
    // ��������͵������ƶ����츴������� //�������� �����Ѿ� �Ǿ��ǱȽ����˵�
    // ���� move �����ñȽ�΢�� �����ƶ����첻��ݻ�ԭ���� �����ϻ��Ǻ�����
    // �� ������ͷ�����������Ƿǳ���ˬ�� ��ԭ��� 
    // unique ptr �� unique ����ԭ��� //�����ඥ ������
    // �����Ͳ��ù���������� ���ҿ��Էǳ��򵥵�д�ͷŵ������߼�
    //auto test = text;
    // make_unique ���ǹ���һ������ Ȼ��make_unique�������ǹ��캯���Ĳ���
    // �ҿ��� sf::Font ��һ�����캯������·��
    // Ȼ�󷵻�ָ����������unique_ptr

    // my_unique_ptr<sf::Font> my_font(std::filesystem::path("SourceHanSerifCN-Medium.otf")); // �Ҳݲ����������ٸ�
    // auto my_test = std::move(my_font);
    // ok �ǳ���
    // �ɹ�
    //û���㿴
    // ��û������ ��Ϊû�����ƣ���
    // ֻ�����������ô��ô���죨��
    //�����˴��Թ����� ɶ���������� û�� ��ʣ��ע��*/
	text->setFillColor(sf::Color::Black);
	circle.setFillColor(sf::Color(245,181,210));
	text->setFont(*font);
	text->setString(L"eat the red apple to extend.And press WASD to move.");
	text->setCharacterSize(24);
    sf::FloatRect rc = text->getLocalBounds();
    sf::Vector2f temp;
    temp.y = rc.position.length() / 2.0f, temp.x = rc.size.length() / 2.0f;
    text->setPosition(sf::Vector2f(800 / 2.0f, 200 / 2.0f));
    text->setOrigin(temp);
    
    
    text_grade = std::make_unique<sf::Text>(*font);
    text_grade->setFillColor(sf::Color::Black);
    text_grade->setFont(*font);
	text_grade->setCharacterSize(24);
	text_grade->setPosition(sf::Vector2f(20, 20));

    start_text = std::make_unique<sf::Text>(*font);
    start_text->setFillColor(sf::Color::Black);
    start_text->setFont(*font);
    start_text->setCharacterSize(24);
    start_text->setString(L"Welcome to the snake game! Press any key to start!");
    rc = start_text->getLocalBounds();
    temp.y = rc.position.length()/ 2.0f, temp.x = rc.size.length() / 2.0f;
    start_text->setPosition(sf::Vector2f(800 / 2.0f, 500 / 2.0f));
    start_text->setOrigin(temp);

    end_text = std::make_unique<sf::Text>(*font);
    end_text->setFillColor(sf::Color::Black);
    end_text->setFont(*font);
    end_text->setCharacterSize(24);

}
void end_text_setting()
{
    std::stringstream temp1;
    if(totlen<100000)
        temp1 << "You die! Press any key to restart!\nYour final grade is: " << grade;
    else
        temp1 << "You win! Press any key to restart!\nYour final grade is: " << grade;
    end_text->setString(temp1.str());
    sf::FloatRect rc = end_text->getLocalBounds();
    sf::Vector2f temp;
    temp.y = rc.position.length() / 2.0f, temp.x = rc.size.length() / 2.0f;
    end_text->setPosition(sf::Vector2f(800 / 2.0f, 500 / 2.0f));
    end_text->setOrigin(temp);
}
void moving()
{
    for (int i = totlen; i > 0; i--)
        snake[i].setPosition(snake[i - 1].getPosition());
    snake[0].setPosition(circle.getPosition());
}
void control()
{
    
    if (state == 1)
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
            circle.move({ 0.f, -3.f*lac }),moving();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
            circle.move({ 0.f, 3.f*lac }),moving();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
            circle.move({ -3.f*lac, 0.f }),moving();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
            circle.move({ 3.f*lac, 0.f }),moving();
    }
}
int rx,ry;
void random()
{
    rx = rand() % 800;
    ry = rand() % 600;
}
void add()
{
    snake[++totlen].setPosition(snake[totlen].getPosition());
    snake[totlen].setRadius(20.f);
    snake[totlen].setFillColor(sf::Color(241, 158, 194));
    snake[totlen].setPosition(snake[totlen - 1].getPosition());
}

double getdistance(double a, double b, double x, double y)
{
    return sqrt((a - x) * (a - x) + (b - y) * (b - y));
}
bool check()
{
    for (int i = 1; i <= totlen; i++)
        if (getdistance(snake[i].getPosition().x, snake[i].getPosition().y, rx, ry) <= 40.0) return 0;
	if (rx <= 40 || ry <= 40 || rx >= 740 || ry >= 540) return 0;
    return 1;
}
bool check_eat()
{
    if (getdistance(circle.getPosition().x, circle.getPosition().y, apple.getPosition().x, apple.getPosition().y) <= 20.0) return 1;
    return 0;
}
void add_apple()
{
    random();
    while (!check()) random();
    sf::Vector2f temp;
    temp.x = rx, temp.y = ry;
    apple.setPosition(temp);
    apple.setFillColor(sf::Color::Red);
}
bool check_boundary()
{
    sf::Vector2f temp=circle.getPosition();
    if (temp.x <= 10 || temp.x >= 750 || temp.y <= 10 || temp.y >= 550) return 0;
    return 1;
}
void grade_to_str()
{
    std::stringstream grade_str;
    grade_str << "Your grade: "<<grade;
    text_grade->setString(grade_str.str());
}
int check_head()
{
       if (rx <= 10 || rx >= 700 || ry <= 10 || ry >= 500) return 0;
       return 1;
}
void bomb_add()
{
    bomb[++totbomb].setFillColor(sf::Color::Black);
    sf::Vector2f temp;
    temp.x = 760.f, temp.y = 560.f;
    bomb[totbomb].setPosition(temp);
    bomb[totbomb].setRadius(15.f);
    bomb[totbomb].setFillColor(sf::Color::Black);
    bomb[totbomb].setOutlineColor(sf::Color::Blue);
    bomb[totbomb].setOutlineThickness(5.f);
}
void start_setting()
{
	totlen = 0;
    snake[0] = circle;
    apple.setRadius(20.f);
    sf::Vector2f temp;
    random();
    while (!check_head()) random();
    temp.x = rx, temp.y = ry;
    //temp.x = 40, temp.y = 40;
	circle.setPosition(temp);
    boundary.setOutlineColor(sf::Color::Black);
	boundary.setOutlineThickness(10.f);
    temp.x = 10, temp.y = 10;
    boundary.setPosition(temp);
    add_apple();
    grade = 0;
    grade_mtx.lock();
    grade_to_str();
    grade_mtx.unlock();
    totbomb = 0;
    //bomb_add();
    bomb_speed = 1;
}
void bomb_move()
{
    sf::Vector2f temp_bomb,temp_head;
    for(int i=1;i<=totbomb;i++)
    {
        temp_bomb = bomb[i].getPosition(), temp_head = circle.getPosition();
        double dis = getdistance(temp_bomb.x, temp_bomb.y, temp_head.x, temp_head.y);
        double sn=temp_bomb.y-temp_head.y,cs=temp_bomb.x-temp_head.x;
        temp_bomb.x -= bomb_speed*0.5*cs / dis;
        temp_bomb.y -= bomb_speed*0.5*sn / dis;
        bomb[i].setPosition(temp_bomb);
    }
}
int check_bomb()
{
    for(int i=1;i<=totbomb;i++)
        if (getdistance(bomb[i].getPosition().x, bomb[i].getPosition().y, circle.getPosition().x, circle.getPosition().y) <= 40.0) return 0;
    return 1;
}
int main()
{
    srand(time(0));
    // create the window
    window.create(sf::VideoMode({ 800, 600 }), "My window");
     // Throws sf::Exception if an error occurs
    // run the program as long as the window is open
    setup_ur_fking_text();
    start_setting();//����Ҫ�ĳ��Ǹ�fking��
    state = 0;
    frameclock.restart();
    window.setActive(false);
    std::thread thread(&renderingThread, &window);
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        while (const std::optional event = window.pollEvent())
        {
            // "close requested" event: we close the window
            if (event->is<sf::Event::Closed>())
                window.close();

        }
        if (frametime.asMilliseconds() > frameclock.getElapsedTime().asMilliseconds())
        {
            int sleep_time = frametime.asMicroseconds() - frameclock.getElapsedTime().asMicroseconds();
            std::this_thread::sleep_for(std::chrono::microseconds(sleep_time));
        }
        frameclock.restart();

        if (state == 0 || state == 2)
        {
            start_event = window.pollEvent();
            if (start_event->is<sf::Event::KeyPressed>()/* || start_event->is<sf::Event::KeyReleased>()*/)
            {
                state = 1;
                bomb_add();
                bomb_time.restart();
            }
        }

        mtx.lock();
        control();
        if (state == 1)
        {
            if (check_eat())
            {
                ++grade;
                grade_to_str();
                add_apple();
                for(int i=1;i<=5;i++) 
                    add();
            }
            bomb_move();
            if (!check_boundary() || !check_bomb() || totlen>=100000)
            {
                state = 2;
                end_text_setting();
                start_setting();
            }
            if (bomb_time_set.asSeconds() <= bomb_time.getElapsedTime().asSeconds())
            {
                bomb_add();
                bomb_speed+=0.5;
                bomb_time.restart();
            }
        }
        mtx.unlock();
    }
    thread.join();
}
