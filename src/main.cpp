#include "audio/AudioEngine.h"
#include "audio/Project.h"
#include "audio/StudioUI.h"

#include <iostream>

using namespace audio;

int main() {
    Project project("Cinematic Sound Design", 48000.0, 120.0);

    Track drums(1, "Drums", 2);
    drums.setGainDb(-3.0f);
    drums.setPan(-0.1f);
    drums.setArmed(true);
    drums.automation("gain").addPoint(0.0, -2.0f);
    drums.automation("gain").addPoint(10.0, 1.5f);

    Track synth(2, "Synth Pads", 2);
    synth.setGainDb(-6.0f);
    synth.setPan(0.2f);
    synth.setSolo(true);

    Track vocals(3, "Vocals", 2);
    vocals.setGainDb(-1.0f);
    vocals.setPan(0.0f);

    project.mixer().addTrack(std::move(drums));
    project.mixer().addTrack(std::move(synth));
    project.mixer().addTrack(std::move(vocals));
    project.mixer().setMasterGainDb(-0.5f);

    project.transport().record();

    AudioEngine engine(project);
    StudioUI ui(project);

    std::cout << "Initializing audio engine...\n\n";
    for (int block = 0; block < 3; ++block) {
        AudioBuffer output = engine.process(256);
        std::cout << "Processed block " << block + 1 << " (" << output.frames() << " frames)\n";
    }
    std::cout << "\n";

    ui.render();

    return 0;
}
