#ifndef PLAYER_H
#define PLAYER_H
#include <raylib.h>
#include "maze.h"
using namespace std;

//当前角色的状态
enum  PlayerState {
	STANDING = -1,
	UP,
	DOWN,
	LEFT,
	RIGHT
};
//角色类
struct player {
private:
	Texture2D texture; //角色纹理
	Vector2 position; //角色位置

	PlayerState state{ PlayerState::STANDING }; //角色状态
	Rectangle curr_frameRec{ 0,0,48.0f,48.0f };//关键帧矩形，用于标注贴图中渲染的区域
	int curr_frame{ 0 }; //当前帧
	float timer{ 0.0f }; //每帧持续时间（计时器，用于计算关键帧动画）
	Coordinate curr_coord{ -1,-1 }; //角色当前所在的地图坐标
	Vector2 target_position; //角色目标位置
	const Maze& maze; //引用迷宫对象
	float base_speed{ 50 }; //角色基础移动速度
	float current_speed{ 50 }; //角色当前移动速度
	int lava_step_count{ 0 }; //踩到熔岩的次数

	void determine_frame_rectangle(); //确定关键帧矩形
	void turn(PlayerState new_state); //转向players(切换角色的状态)
	void control_player_update(); //控制角色移动
	void update_position(float delta_time);//计算并更新角色位置

public:
	player(const Maze&); //构造函数
	~player(); //析构函数
	Coordinate get_curr_coord()const { return curr_coord; } //获取当前坐标                                             
	void update(); //根据按键来更新角色状态
	void draw()const; //绘制角色
	void reset();//添加的rest函数，用于重置玩家位置
	// 获取踩到熔岩的次数
	int get_lava_step_count() const { return lava_step_count; }

	// 重置熔岩计数
	void reset_lava_count() { lava_step_count = 0; }
};
#endif // PLAYER_H