export module SettingsScreen;
import ScreenController;
import <string>;
import SpeedSettingScreen;
import <vector>;

export class SettingsScreen :public Screen
{

	// ����
	virtual void Render(GameBuffer& buf)
	{
		std::string str = "ѡ��:\n";
		str.append("J - ˫Ѻ����\nS - �ٶ����ý���");
		//str.append(game->Settings[JumpHelper].Get<bool>() ? "[ON]" : "[OFF]");
		str.append("\n");
		buf.DrawString(str, 0, 0, {}, {});
	}
	virtual void Key(KeyEventArgs kea)
	{
		if (kea.Pressed)
		{
			if (kea.Key == ConsoleKey::Escape)
			{
				parent->Back();
			}
			if (kea.Key == ConsoleKey::S)
			{
				parent->Navigate<SpeedSettingScreen>();
				return;
			}
		}
	}
};