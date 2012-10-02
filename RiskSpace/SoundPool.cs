using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Media;
using System.Windows.Media;

namespace RiskSpace
{
    public enum GameSound
    {
        AddArmy,
        AttackChooseSource,
        AttackFail,
        AttackL1,
        AttackL2,
        AttackL3,
        AttackWin,
        Error,
        Success,
        RoundPass,
    }

    public class SoundPool
    {

        private Dictionary<GameSound, SoundPlayer> soundPlayers;
        private MediaPlayer bgmPlayer;

        public SoundPool()
        {
            soundPlayers = new Dictionary<GameSound, SoundPlayer>();

            foreach (GameSound soundName in Enum.GetValues(typeof(GameSound)))
            {
                SoundPlayer player = new SoundPlayer("Sound/" + soundName.ToString() + ".wav");
                player.Load();
                soundPlayers.Add(soundName, player);
            }

            
        }

        public void Play(GameSound gameSound)
        {
            soundPlayers[gameSound].Play();
        }

        public void PlayBgm()
        {
            /*bgmPlayer = new MediaPlayer();
            bgmPlayer.Open(new Uri("Sound/bgm.mp3", UriKind.Relative));
            bgmPlayer.Play();*/
        }
    }
}
