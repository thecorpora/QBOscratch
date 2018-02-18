## TTS: Pico2Wave

### Installation

sudo apt-get install libttspico-utils

### Configuration

Change "Python projects/config.yml" tts parameters to "pico2wave" for activate it

## Play from Youtube

### Installation

Install youtube-dl (download m4a)
https://github.com/rg3/youtube-dl/blob/master/README.md#how-do-i-update-youtube-dl

sudo wget https://yt-dl.org/latest/youtube-dl -O /usr/local/bin/youtube-dl
sudo chmod a+x /usr/local/bin/youtube-dl
hash -r

apt-get install ffmpeg

### Use it from PyCmd

python2.7 PiCmd.py -c youtube -y Zs81kDyjTP8
(-y is VIDEO_YOUTUBE_ID parameter)