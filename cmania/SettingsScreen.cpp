#include "ScreenController.h"
#include <string>
#include "SpeedSettingScreen.h"
#include <vector>
#include "SettingsScreen.h"
class SettingsScreen :public Screen
{
	int scroll = 0;
	virtual void Render(GameBuffer& buf)
	{
		std::string line = "ѡ��(�����¼�ͷ��ҳ):\n";
		line.push_back('[');
		line.push_back(game->Settings["JumpHelper"].Get<bool>() ? 'x' : ' ');
		line.push_back(']');
		line.append("˫Ѻ����(J)\n");
		line.push_back('[');
		line.push_back(game->Settings["NoBmpHs"].Get<bool>() ? 'x' : ' ');
		line.push_back(']');
		line.append("�ر����������(H)\n");
		line.push_back('[');
		line.push_back(game->Settings["WtMode"].Get<bool>() ? 'x' : ' ');
		line.push_back(']');
		line.append("Windows Terminalģʽ(�رճ����Ʒ�)(W)\n");
		line.push_back('[');
		line.push_back(game->Settings["NoBg"].Get<bool>() ? 'x' : ' ');
		line.push_back(']');
		line.append("�رձ���(B)\n");
		line.push_back('[');
		line.push_back(game->Settings["TailHs"].Get<bool>() ? 'x' : ' ');
		line.push_back(']');
		line.append("����β������(T)\n");
		line.append("�ٶ�������(S)\n");
		line.append("����Songs·��(R)\n");
		for (size_t i = 0; i < buf.Height; i++)
		{
			line.push_back('\n');
		}
		line.append("Author: Telecomadm1145 ( https://github.com/telecomadm1145 )\n");
		buf.DrawString(line, 0,scroll, {}, {});
	}
	virtual void Key(KeyEventArgs kea)
	{
		if (kea.Pressed)
		{
			if (kea.Key == ConsoleKey::Escape)
			{
				parent->Back();
				return;
			}
			if (kea.Key == ConsoleKey::DownArrow)
			{
				scroll++;
				return;
			}
			if (kea.Key == ConsoleKey::UpArrow)
			{
				scroll--;
				return;
			}
			if (kea.Key == ConsoleKey::J)
			{
				game->Settings["JumpHelper"].Set(!game->Settings["JumpHelper"].Get<bool>());
				game->Settings.Write();
				return;
			}
			if (kea.Key == ConsoleKey::H)
			{
				game->Settings["NoBmpHs"].Set(!game->Settings["NoBmpHs"].Get<bool>());
				game->Settings.Write();
				return;
			}
			if (kea.Key == ConsoleKey::T)
			{
				game->Settings["TailHs"].Set(!game->Settings["TailHs"].Get<bool>());
				game->Settings.Write();
				return;
			}
			if (kea.Key == ConsoleKey::W)
			{
				game->Settings["WtMode"].Set(!game->Settings["WtMode"].Get<bool>());
				game->Settings.Write();
				return;
			}
			if (kea.Key == ConsoleKey::B)
			{
				game->Settings["NoBg"].Set(!game->Settings["NoBg"].Get<bool>());
				game->Settings.Write();
				return;
			}
			if (kea.Key == ConsoleKey::S)
			{
				parent->Navigate(MakeSpeedSettingScreen());
				return;
			}
			if (kea.Key == ConsoleKey::R)
			{
				game->Settings["SongsPath"].SetArray("",0);
				game->Settings.Write();
				return;
			}
		}
	}
};

Screen* MakeSettingsScreen()
{
	return new SettingsScreen();
}
