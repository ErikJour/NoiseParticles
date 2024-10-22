#pragma once

#include "PluginProcessor.h"
#include<juce_gui_extra/juce_gui_extra.h>
#include<juce_audio_utils/juce_audio_utils.h>
#include "DSP/NoiseGenerator.h"

//==============================================================================

namespace NoiseParticles
{

class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void resized() override;

private:
    using Resource = juce::WebBrowserComponent::Resource;  

    std::optional<Resource> getResource (const juce::String& url);

    void nativeFunction (const juce::Array<juce::var>& args,
                         juce::WebBrowserComponent::NativeFunctionCompletion completion);

    juce::WebBrowserComponent webViewGui;

    AudioPluginAudioProcessor& processorRef;

    NoiseGenerator noiseGenerator;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
}