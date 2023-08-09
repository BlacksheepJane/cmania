#include "SongSelectScreen.h"
#include "ScreenController.h"
#include "SettingsScreen.h"
#include "MainMenuScreen.h"

class MainMenuScreen : public Screen {
	virtual void Render(GameBuffer& buf) {
		buf.DrawString("���� Enter ����ѡ�����\n���� O ����������", 0, 0, {}, {});
	}
	virtual void Key(KeyEventArgs kea) {
		if (kea.Pressed) {
			if (kea.Key == ConsoleKey::Enter) {
				parent->Navigate(MakeSongSelectScreen());
			}
			if (kea.Key == ConsoleKey::O) {
				parent->Navigate(MakeSettingsScreen());
			}
		}
	}
};

Screen* MakeMainMenuScreen() {
	return new MainMenuScreen();
}
