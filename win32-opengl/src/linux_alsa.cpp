struct linux_sound_output_t
{
    snd_pcm_t *pcm;
    u32 samplesPerSecond;
    u16 channels;
};

internal void Linux_InitAlsa(linux_sound_output_t *soundOutput)
{
    snd_pcm_open(&soundOutput->pcm, "default", SND_PCM_STREAM_PLAYBACK, 0);
    snd_pcm_set_params(soundOutput->pcm, SND_PCM_FORMAT_S16_LE, SND_PCM_ACCESS_RW_INTERLEAVED,
                       soundOutput->channels, soundOutput->samplesPerSecond, 1, 16667);
}

internal void Linux_FillSoundBuffer(u32 samplesToWrite, i16 *samples, linux_sound_output_t *output)
{
    snd_pcm_writei(output->pcm, samples, samplesToWrite);
}
