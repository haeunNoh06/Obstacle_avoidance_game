#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>//SoundBuffer ���
//#include <SFML/Audio/SoundSource.hpp>

using namespace sf;

enum jump_state {
	RUN,
	JUMP
};

struct Player {
	RectangleShape sprite;
	int x;
	int y;
	int life;

	// �ִϸ��̼� ���� ����
	int fps;// �ʴ� ������ ���� (frame per sec)
	int idx;// �ִϸ��̼� �ε���
	int frames;// �ִϸ��̼� frame ��
	long ani_time;// �ִϸ��̼��� �ٲ� ���� �ð�
	long ani_delay;// ȭ�� ��ȯ�� ��

	int speed;// �ӵ�
	//������ ���õ� ����
	enum jump_state jump_state;
	int is_jumping;// ���� ����
	int jumping_time;// ������ �����ϴ� �ð�
	int jump_speed;// ���� �ӵ�
};

// ��ֹ�
struct Obstacle {
	RectangleShape sprite;
	int speed;
	int is_appeared;// ��Ÿ���°�
};

struct Textures {
	Texture run[10];
	Texture jump[10];
	Texture bg;
	Texture obstacle;// ��ֹ�
};

//obj1�� obj2�� �浹 ���� �浹�ϸ� 1�� ��ȯ �ƴϸ� 0���� ��ȯ
int is_collide(RectangleShape obj1, RectangleShape obj2) {
	return obj1.getGlobalBounds().intersects(obj2.getGlobalBounds());
}

const int OBSTACLE_NUM = 3;// ��ֹ� ��
const int GRAVITY = 3;// �߷�  
const int PLATFORM_Y = 600;// �� �ٴ��� y��ǥ
const int W_WIDTH = 1200, W_HEIGHT = 800;// â�� ũ��

int main(void)
{
	RenderWindow window(VideoMode(1200, 800), "Animation");
	window.setFramerateLimit(60);

	srand(time(0));

	long start_time;
	long spent_time;// ���� ���� �ð�

	int is_over = 0;// ���� ����

	struct Textures t;
	t.bg.loadFromFile("./resources/imgs/background.png");
	t.obstacle.loadFromFile("./resources/imgs/obstacle.png");
	t.run[0].loadFromFile("./resources/imgs/Run__000.png");
	t.run[1].loadFromFile("./resources/imgs/Run__001.png");
	t.run[2].loadFromFile("./resources/imgs/Run__002.png");
	t.run[3].loadFromFile("./resources/imgs/Run__003.png");
	t.run[4].loadFromFile("./resources/imgs/Run__004.png");
	t.run[5].loadFromFile("./resources/imgs/Run__005.png");
	t.run[6].loadFromFile("./resources/imgs/Run__006.png");
	t.run[7].loadFromFile("./resources/imgs/Run__007.png");
	t.run[8].loadFromFile("./resources/imgs/Run__008.png");
	t.run[9].loadFromFile("./resources/imgs/Run__009.png");
	t.jump[0].loadFromFile("./resources/imgs/Jump__000.png");
	t.jump[1].loadFromFile("./resources/imgs/Jump__001.png");
	t.jump[2].loadFromFile("./resources/imgs/Jump__002.png");
	t.jump[3].loadFromFile("./resources/imgs/Jump__003.png");
	t.jump[4].loadFromFile("./resources/imgs/Jump__004.png");
	t.jump[5].loadFromFile("./resources/imgs/Jump__005.png");
	t.jump[6].loadFromFile("./resources/imgs/Jump__006.png");
	t.jump[7].loadFromFile("./resources/imgs/Jump__007.png");
	t.jump[8].loadFromFile("./resources/imgs/Jump__008.png");
	t.jump[9].loadFromFile("./resources/imgs/Jump__009.png");

	// Player
	struct Player player;
	player.jump_state = RUN;
	player.fps = 10;
	player.sprite.setTexture(&t.run[0]);
	player.sprite.setSize(Vector2f(90, 120));
	player.sprite.setPosition(200, 0);
	player.x = player.sprite.getPosition().x;// �÷��̾� x��ǥ
	player.y = player.sprite.getPosition().y;// �÷��̾� y��ǥ
	player.frames = 10;
	player.ani_delay = 1000 / player.frames / 2;// 0.5�ʸ��� ����
	player.speed = 5;
	player.jump_speed = GRAVITY * 4;// ������ �ӵ��� �ö󰡰ų� ������
	player.life = 5;

	// ��ֹ�
	struct Obstacle ob[OBSTACLE_NUM];
	int ob_idx = 0;// ��ֹ��� ���� ������ ������ų ��
	int ob_delay = 1500;
	int ob_appeared_time;// ��ֹ��� ��Ÿ�� �ð�
	for (int i = 0; i < OBSTACLE_NUM; i++)
	{
		ob[i].sprite.setSize(Vector2f(70, 73));
		ob[i].sprite.setTexture(&t.obstacle);
		ob[i].sprite.setPosition(W_WIDTH - 70, PLATFORM_Y - 73);
		ob[i].speed = -(rand() % 5 + 1);
		ob[i].is_appeared = 0;
	}

	//���
	Sprite bg_sprite;
	bg_sprite.setTexture(t.bg);
	bg_sprite.setPosition(0, 0);

	// text ��Ʈ
	Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\Candara.ttf");//C����̺꿡 �ִ� ��Ʈ ��������

	// �ؽ�Ʈ
	Text text;
	char info[40];
	text.setFont(font);//��Ʈ ����
	text.setCharacterSize(24);//��Ʈ ũ��
	text.setFillColor(Color(0, 0, 0));//RGB�� ��� ǥ��
	text.setPosition(10, 10);//�ؽ�Ʈ ��ġ 0,0

	start_time = clock();
	player.ani_time = start_time;
	player.jumping_time = start_time;
	player.is_jumping = 0;

	ob_appeared_time = start_time;// ���� �ð�

	while (window.isOpen())
	{
		spent_time = clock() - start_time;

		player.x = player.sprite.getPosition().x;// �÷��̾� x��ǥ
		player.y = player.sprite.getPosition().y;// �÷��̾� y��ǥ

		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				window.close();
				break;
			case Event::KeyPressed:
				if (event.key.code == Keyboard::Space)
				{
					// ���� ���
					if (player.is_jumping == 0)
					{
						player.jumping_time = spent_time;
					}
					player.is_jumping = 1;// true
				}
			}
		}



		// ���� �ð��� ������ ����
		sprintf(info, "time: %d, life: %d\n", spent_time / 1000, player.life);

		text.setString(info);

		// 1.5�� ���� ��ֹ� ������
		if (spent_time - ob_appeared_time > ob_delay)
		{
			if (ob[ob_idx].is_appeared == 0)
			{
				ob_appeared_time = spent_time;
				ob[ob_idx].is_appeared = 1;
				ob[ob_idx].sprite.setPosition(W_WIDTH - 70, PLATFORM_Y - 73);
				ob_idx++;// �Ŀ� ����
				ob_idx = ob_idx % OBSTACLE_NUM;// 3 ��� 0���� �ٲ�
			}

		}

		// �浹 ó��
		for (int i = 0; i < OBSTACLE_NUM; i++)
		{
			// �÷��̾�� ��ֹ��� �浹�ϸ�
			if (is_collide(player.sprite, ob[i].sprite))
			{
				player.life -= 1;
			}
		}

		// �÷��̾ ������
		if (player.life <= 0)
		{
			is_over = 1;
		}

		// ���� ����
		if (is_over == 1)
		{
			break;
		}

		for (int i = 0; i < OBSTACLE_NUM; i++)
		{
			if (ob[i].is_appeared == 1)
			{
				ob[i].sprite.move(ob[ob_idx].speed, 0);
			}

			// ��ֹ��� ���� ������
			if (ob[i].sprite.getPosition().x < 0)
			{
				ob[i].is_appeared = 0;
			}
		}
		if (player.is_jumping == 1)
		{
			player.idx = 0;
			player.sprite.move(0, -player.jump_speed - 5);
		}
		// 0.1�ʸ��� �ִϸ��̼� �׸��� �ٲ�
		while (spent_time - player.ani_time > 1000 / player.ani_delay)
		{
			// �ݺ��Ϸ���
			player.ani_time = spent_time;
			player.sprite.setTexture(&t.run[player.idx % player.frames]);
			player.idx++;
		}

		//�ʿ��ϴٸ� 1000�� ���߿� ���� ó���Ұ�
		if (spent_time - player.jumping_time > 500)
		{
			player.is_jumping = 0;
			player.jump_state = RUN;
		}

		player.sprite.move(0, GRAVITY * 3);// �߷� ����

		// �÷��̾ ���ٴڿ� ���� �ϸ�
		if (player.sprite.getPosition().y + player.sprite.getSize().y > PLATFORM_Y)// �� �ٴ��� y��ǥ�� �÷��̾��� y��ǥ�� ���� �÷��̾ ���� ������
		{
			// �� �̻� ������ �� ���� �Ѵ�
			player.sprite.setPosition(player.sprite.getPosition().x, PLATFORM_Y - player.sprite.getSize().y);
		}

		window.clear(Color::Blue);

		window.draw(bg_sprite);

		for (int i = 0; i < OBSTACLE_NUM; i++)
		{
			// ��ֹ��� ��Ÿ���ٸ�
			if (ob[i].is_appeared == 1)
			{
				window.draw(ob[i].sprite);
			}
		}

		window.draw(player.sprite);

		window.draw(text);

		window.display();
	}

	return 0;
}