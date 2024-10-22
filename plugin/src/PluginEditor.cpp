#include "NoiseParticles/PluginEditor.h"
#include "NoiseParticles/PluginProcessor.h"


//==============================================================================
namespace {
auto streamToVector (juce::InputStream& stream) {
    using namespace juce;

    std::vector<std::byte> result ((size_t) stream.getTotalLength());
    stream.setPosition (0);
    [[maybe_unused]] const auto bytesRead = stream.read (result.data(), result.size());
    jassert (bytesRead == (ssize_t) result.size());
    return result;
}

const char* getMimeForExtension (const juce::String& extension)
{
    using namespace juce;
    static const std::unordered_map<String, const char*> mimeMap =
    {
        { { "htm"   },  "text/html"                },
        { { "html"  },  "text/html"                },
        { { "txt"   },  "text/plain"               },
        { { "jpg"   },  "image/jpeg"               },
        { { "jpeg"  },  "image/jpeg"               },
        { { "svg"   },  "image/svg+xml"            },
        { { "ico"   },  "image/vnd.microsoft.icon" },
        { { "json"  },  "application/json"         },
        { { "png"   },  "image/png"                },
        { { "css"   },  "text/css"                 },
        { { "map"   },  "application/json"         },
        { { "js"    },  "text/javascript"          },
        { { "woff2" },  "font/woff2"               }
    };

    if (const auto it = mimeMap.find (extension.toLowerCase()); it != mimeMap.end())
        return it->second;

    jassertfalse;
    return "";
}
}

//==============================================================================

namespace NoiseParticles
{


AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p),
    webViewGui{juce::WebBrowserComponent::Options{}
    .withResourceProvider([this](const auto& url){
        return getResource(url);})
        .withNativeIntegrationEnabled()
        .withUserScript(R"(console.log("C++ Backend here: This is run before any other loading happens.");)")
        .withNativeFunction(
            juce::Identifier{"nativeFunction"},
            [this](const juce::Array<juce::var>& args,
            juce::WebBrowserComponent::NativeFunctionCompletion completion){
               nativeFunction(args, std::move(completion)); 
            }
        )}

{
    juce::ignoreUnused (processorRef);

    //WEBVIEW GUI
    webViewGui.goToURL("http://localhost:5173/");
    addAndMakeVisible(webViewGui);

    //WINDOW SETUP
    setResizable(true, true);    
    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================

void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    webViewGui.setBounds(bounds);

}
   auto AudioPluginAudioProcessorEditor::getResource (const juce::String& url) -> std::optional<Resource> {

    std::cout << url << std::endl;

    // static const auto resourceFileRoot = juce::File{R"(/Users/erikjourgensen/Desktop/Weekly Projects/Week 1 - Noise Particles/plugin/UI/public)"};

    static const auto resourceFileRoot = juce::File{R"(/Users/erikjourgensen/Desktop/Weekly Projects/Week 1 - Noise Particles/plugin/UI/public)"};

    const auto resourceToRetrieve = url == "/" ? "index.html" : url.fromFirstOccurrenceOf ("/", false, false);

    const auto resource = resourceFileRoot.getChildFile(resourceToRetrieve).createInputStream();

    if (resource){
        const auto extension = resourceToRetrieve.fromLastOccurrenceOf(".", false, false);
        return Resource{streamToVector(*resource), getMimeForExtension(extension)};
    }

    return std::nullopt;
}
void AudioPluginAudioProcessorEditor::nativeFunction(const juce::Array<juce::var>& args,
                                                     juce::WebBrowserComponent::NativeFunctionCompletion completion)
{
    
  if (args.size() == 1 && args[0].isArray())

  {
    auto* arrayArg = args[0].getArray();

    if (arrayArg != nullptr && arrayArg->size() >=2) 
    {
        std::string paramType = arrayArg->getReference(0).toString().toStdString();

        float value = static_cast<float>(arrayArg->getReference(1));

        if (paramType == "NoiseLevel")
        {
            processorRef.setNoiseLevel(value);
        }

        else if (paramType == "Gain")
        {
            processorRef.setNoiseLevel(value);
        }
        else{
            std::cout << "Unknown parameter type: " << paramType << std::endl;
        }
    }

  }

}
}
