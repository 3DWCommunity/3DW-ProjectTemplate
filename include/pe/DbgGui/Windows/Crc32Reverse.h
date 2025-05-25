#pragma once

#include "pe/DbgGui/IComponent.h"

namespace pe {
    namespace gui {

        class Crc32Reverse : public IComponent {
        public:
            Crc32Reverse();

            void update() override;
            void draw() override;
        };

    } // namespace gui
} // namespace pe
