#pragma once
class GameManager
{
private:
	static GameManager* instance;
public:
#pragma region Singleton

	static GameManager* GetInstance();

#pragma endregion

#pragma region Game Loop

	void Init();
	void Update();
	void Draw();

#pragma endregion
};