#include "polos/core/resource/importers/image_importer_stb.h"

#include <stb_image.h>

#include "polos/containers/delegate.h"

namespace polos::resource
{
    ImageImporterStb::ImageImporterStb()
        : ResourceLoader<image>(
            Delegate<image*(std::string, import_options<image>, image*)>::template From<ImageImporterStb, &ImageImporterStb::Import>(this)
          , Delegate<bool(std::string)>::template From<ImageImporterStb, &ImageImporterStb::CanImport>(this)
        )
    {
    }

    image* ImageImporterStb::Import(std::string p_Path, import_options<image>, image* p_Ptr)
    {
        glm::ivec2 dim;
        int32 channels;

        cstring path = p_Path.c_str();

        stbi_set_flip_vertically_on_load(1);
        byte* data = stbi_load(path, &dim.x, &dim.y, &channels, 0);

        if (data == nullptr)
        {
            LOG_ENGINE_ERROR("[ImageImporterStb::Import] Couldn't load image from path: \"{}\"", p_Path);
            return nullptr;
        }

        std::size_t data_size = dim.x * dim.y * channels;

        p_Ptr->channels = channels;
        p_Ptr->dimensions = dim;
        p_Ptr->data = DArray<byte>(data, std::next(data, data_size));

        return p_Ptr;
    }

    bool ImageImporterStb::CanImport(std::string p_Path)
    {
        int32 x;
        int32 y;
        int32 channels;
        return stbi_info(p_Path.c_str(), &x, &y, &channels);
    }
} // namespace polos::resource