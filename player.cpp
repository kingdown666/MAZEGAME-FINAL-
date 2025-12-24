#include"player.h"
#include<format>
#include<iostream>
#include"raymath.h"
#include<cmath>
#include"raylib.h"
#define TILE_HEIGHT 48.0f 

constexpr float frame_time{ 0.1f }; //每帧持续时间
player::player(const Maze& m) :maze(m) {
	Image player_image = LoadImage("resources/character.png");
	texture = LoadTextureFromImage(player_image);
	UnloadImage(player_image);

	auto start_coord = maze.get_start_Coord();
	curr_coord = start_coord;
	position = maze.get_tile_position(start_coord);

};
player::~player() {
	//将角色从内存中删除
	UnloadTexture(texture);
}
//rest函数实现
void player::reset() {
	auto start_coord = maze.get_start_Coord();
	curr_coord = start_coord;
	position = maze.get_tile_position(start_coord);
	state = STANDING;
	curr_frame = 0;
	timer = 0.0f;
	target_position = position;
	current_speed = base_speed; // 重置速度
	lava_step_count = 0; // 重置熔岩计数
}
void player::update() {
	control_player_update();
	if (state == STANDING) { return; }
	determine_frame_rectangle();
	update_position(GetFrameTime());  // 传入帧时间更新位置
}
void player::update_position(float delta_time) {
	// 向目标位置移动（使用current_speed而不是固定的speed）
	position = Vector2MoveTowards(position, target_position, current_speed * delta_time);
	// 如果到达目标位置，恢复站立状态
	if (Vector2Equals(position, target_position)) {
		// 计算目标位置对应的坐标（从目标位置反推格子坐标）
		int target_x = static_cast<int>(target_position.x / TILE_WIDTH);
		int target_y = static_cast<int>(target_position.y / TILE_HEIGHT);
		Coordinate new_coord = { target_x, target_y };

		// 检查是否移动到了新的格子
		if (new_coord.x != curr_coord.x || new_coord.y != curr_coord.y) {
			// 更新当前坐标
			curr_coord = new_coord;

			// 检查当前格子的类型
			TileType current_tile = maze.get_tile_type(curr_coord);

			// 功能(a): 当小人处于草地时，行走速度降低到原速度的三分之一
			if (current_tile == GRASS) {
				current_speed = base_speed / 3.0f;
			}
			else {
				current_speed = base_speed; // 恢复原速度
			}

			// 功能(b): 当小人踩到熔岩地板时，记录次数
			if (current_tile == LAVA) {
				lava_step_count++;
				std::cout << "Stepped on lava! Count: " << lava_step_count << std::endl;
			}
		}

		state = STANDING;
	}
}

void player::draw()const {
	DrawTextureRec(texture, curr_frameRec, position, WHITE);
}
void player::determine_frame_rectangle() {
	timer += GetFrameTime();
	if (timer >= frame_time) {
		timer = 0;
		++curr_frame;
		if (curr_frame > 2) { curr_frame = 0; }

	}
	curr_frameRec.x = curr_frame * (texture.width / 3);
}

void player::turn(const PlayerState to_state) {
	if (state == to_state) { return; } // 状态不变则直接返回
	state = to_state;                  // 更新角色状态（朝向）
	curr_frame = 0;                    // 重置动画帧为第1帧
	timer = 0.0F;                      // 重置帧计时器

	// 关键：根据状态映射贴图行（贴图行号从0开始，对应需求：第1行=上、第2行=左、第3行=右、第4行=下）
	switch (to_state) {
	case DOWN:    curr_frameRec.y = 0 * TILE_HEIGHT;    break; // 贴图第1行（索引0）= 上
	case LEFT:  curr_frameRec.y = 1 * TILE_HEIGHT;    break; // 贴图第2行（索引1）= 左
	case RIGHT: curr_frameRec.y = 2 * TILE_HEIGHT;    break; // 贴图第3行（索引2）= 右
	case UP:  curr_frameRec.y = 3 * TILE_HEIGHT;    break; // 贴图第4行（索引3）= 下
	default:    curr_frameRec.y = 0;                  break; // 默认向上
	}
	curr_frameRec.x = 0; // 重置帧的x坐标（从当前朝向的第1帧开始）
}

void player::control_player_update() {
	if (state != STANDING) { return; }

	Coordinate target_coor{ curr_coord };

	// 检测是否按下方向键
	if (IsKeyDown(KEY_DOWN)) {
		if (state != DOWN) { turn(DOWN); }
		++target_coor.y;
	}
	else if (IsKeyDown(KEY_LEFT)) {
		if (state != LEFT) { turn(LEFT); }
		--target_coor.x;
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		if (state != RIGHT) { turn(RIGHT); }
		++target_coor.x;
	}
	else if (IsKeyDown(KEY_UP)) {
		if (state != UP) { turn(UP); }
		--target_coor.y;
	}
	else {
		return;
	}

	if (maze.get_tile_type(target_coor) == WALL) { return; }

	//更新目标位置和当前位置
	target_position = maze.get_tile_position(target_coor);
}