#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>//SoundBuffer 사용
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

	// 애니메이션 관련 변수
	int fps;// 초당 프레임 갯수 (frame per sec)
	int idx;// 애니메이션 인덱스
	int frames;// 애니메이션 frame 수
	long ani_time;// 애니메이션이 바뀔 때의 시각
	long ani_delay;// 화면 전환의 텀

	int speed;// 속도
	//점프와 관련된 변수
	enum jump_state jump_state;
	int is_jumping;// 점프 상태
	int jumping_time;// 점프를 시작하는 시각
	int jump_speed;// 점프 속도
};

// 장애물
struct Obstacle {
	RectangleShape sprite;
	int speed;
	int is_appeared;// 나타났는가
};

struct Textures {
	Texture run[10];
	Texture jump[10];
	Texture bg;
	Texture obstacle;// 장애물
};

//obj1과 obj2의 충돌 여부 충돌하면 1로 반환 아니면 0으로 반환
int is_collide(RectangleShape obj1, RectangleShape obj2) {
	return obj1.getGlobalBounds().intersects(obj2.getGlobalBounds());
}

const int OBSTACLE_NUM = 3;// 장애물 수
const int GRAVITY = 3;// 중력  
const int PLATFORM_Y = 600;// 땅 바닥의 y좌표
const int W_WIDTH = 1200, W_HEIGHT = 800;// 창의 크기

int main(void)
{
	RenderWindow window(VideoMode(1200, 800), "Animation");
	window.setFramerateLimit(60);

	srand(time(0));

	long start_time;
	long spent_time;// 게임 진행 시간

	int is_over = 0;// 종료 여부

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
	player.x = player.sprite.getPosition().x;// 플레이어 x좌표
	player.y = player.sprite.getPosition().y;// 플레이어 y좌표
	player.frames = 10;
	player.ani_delay = 1000 / player.frames / 2;// 0.5초마다 걸음
	player.speed = 5;
	player.jump_speed = GRAVITY * 4;// 일정한 속도로 올라가거나 내려감
	player.life = 5;

	// 장애물
	struct Obstacle ob[OBSTACLE_NUM];
	int ob_idx = 0;// 장애물이 나올 때마다 증가시킬 것
	int ob_delay = 1500;
	int ob_appeared_time;// 장애물이 나타난 시각
	for (int i = 0; i < OBSTACLE_NUM; i++)
	{
		ob[i].sprite.setSize(Vector2f(70, 73));
		ob[i].sprite.setTexture(&t.obstacle);
		ob[i].sprite.setPosition(W_WIDTH - 70, PLATFORM_Y - 73);
		ob[i].speed = -(rand() % 5 + 1);
		ob[i].is_appeared = 0;
	}

	//배경
	Sprite bg_sprite;
	bg_sprite.setTexture(t.bg);
	bg_sprite.setPosition(0, 0);

	// text 폰트
	Font font;
	font.loadFromFile("C:\\Windows\\Fonts\\Candara.ttf");//C드라이브에 있는 폰트 가져오기

	// 텍스트
	Text text;
	char info[40];
	text.setFont(font);//폰트 세팅
	text.setCharacterSize(24);//폰트 크기
	text.setFillColor(Color(0, 0, 0));//RGB로 흰색 표현
	text.setPosition(10, 10);//텍스트 위치 0,0

	start_time = clock();
	player.ani_time = start_time;
	player.jumping_time = start_time;
	player.is_jumping = 0;

	ob_appeared_time = start_time;// 시작 시간

	while (window.isOpen())
	{
		spent_time = clock() - start_time;

		player.x = player.sprite.getPosition().x;// 플레이어 x좌표
		player.y = player.sprite.getPosition().y;// 플레이어 y좌표

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
					// 점프 기능
					if (player.is_jumping == 0)
					{
						player.jumping_time = spent_time;
					}
					player.is_jumping = 1;// true
				}
			}
		}



		// 시작 시간은 변하지 않음
		sprintf(info, "time: %d, life: %d\n", spent_time / 1000, player.life);

		text.setString(info);

		// 1.5초 마다 장애물 나오기
		if (spent_time - ob_appeared_time > ob_delay)
		{
			if (ob[ob_idx].is_appeared == 0)
			{
				ob_appeared_time = spent_time;
				ob[ob_idx].is_appeared = 1;
				ob[ob_idx].sprite.setPosition(W_WIDTH - 70, PLATFORM_Y - 73);
				ob_idx++;// 후에 증가
				ob_idx = ob_idx % OBSTACLE_NUM;// 3 대신 0으로 바뀜
			}

		}

		// 충돌 처리
		for (int i = 0; i < OBSTACLE_NUM; i++)
		{
			// 플레이어와 장애물이 충돌하면
			if (is_collide(player.sprite, ob[i].sprite))
			{
				player.life -= 1;
			}
		}

		// 플레이어가 죽으면
		if (player.life <= 0)
		{
			is_over = 1;
		}

		// 게임 종료
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

			// 장애물이 벽에 닿으면
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
		// 0.1초마다 애니메이션 그림이 바뀜
		while (spent_time - player.ani_time > 1000 / player.ani_delay)
		{
			// 반복하려고
			player.ani_time = spent_time;
			player.sprite.setTexture(&t.run[player.idx % player.frames]);
			player.idx++;
		}

		//필요하다면 1000을 나중에 변수 처리할것
		if (spent_time - player.jumping_time > 500)
		{
			player.is_jumping = 0;
			player.jump_state = RUN;
		}

		player.sprite.move(0, GRAVITY * 3);// 중력 적용

		// 플레이어가 땅바닥에 착지 하면
		if (player.sprite.getPosition().y + player.sprite.getSize().y > PLATFORM_Y)// 땅 바닥의 y좌표와 플레이어의 y좌표를 빼야 플레이어가 땅에 서있음
		{
			// 더 이상 내려갈 수 없게 한다
			player.sprite.setPosition(player.sprite.getPosition().x, PLATFORM_Y - player.sprite.getSize().y);
		}

		window.clear(Color::Blue);

		window.draw(bg_sprite);

		for (int i = 0; i < OBSTACLE_NUM; i++)
		{
			// 장애물이 나타났다면
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