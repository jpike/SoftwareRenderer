#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Graphics/Modeling/WavefrontObjectModel.h"
#include "Graphics/Modeling/WavefrontMaterial.h"

namespace GRAPHICS::MODELING
{
    /// Attempts to load the model from the specified .obj file.
    /// Any additional referenced files are automatically loaded to ensure a complete model is loaded.
    /// @param[in]  obj_filepath - The path of the .obj file to load.
    /// @return The 3D model, if successfull loaded; null otherwise.
    std::optional<Object3D> WavefrontObjectModel::Load(const std::filesystem::path& obj_filepath)
    {
        // OPEN THE FILE.
        std::ifstream obj_file(obj_filepath);
        bool obj_file_opened = obj_file.is_open();
        if (!obj_file_opened)
        {
            return std::nullopt;
        }

        // READ IN THE DATA FROM THE .OBJ FILE.
        // Note that this reading may not yet be fully robust.
        // It only handles the absolute minimum as currently needed for basic demos.
        constexpr char SPACE_SEPARATOR = ' ';
        std::vector<std::filesystem::path> material_filenames;
        std::vector<MATH::Vector3f> vertices;
        std::string line;
        while (std::getline(obj_file, line))
        {
            // SKIP OVER ANY BLANK LINES.
            bool is_blank_line = line.empty();
            if (is_blank_line)
            {
                continue;
            }

            // SKIP OVER ANY COMMENT LINES.
            constexpr char OBJ_COMMENT_CHARACTER = '#';
            bool is_comment_line = line.starts_with(OBJ_COMMENT_CHARACTER);
            if (is_comment_line)
            {
                continue;
            }

            // TRACK ANY MATERIALS THAT NEED TO BE LOADED SEPARATELY.
            const std::string MATERIAL_LIBRARY_KEYWORD = "mtllib";
            bool is_material_file_line = line.starts_with(MATERIAL_LIBRARY_KEYWORD);
            if (is_material_file_line)
            {
                // FIND WHERE THE MATERIAL FILENAME STARTS IN THE LINE.
                std::size_t space_separator_index = line.find(SPACE_SEPARATOR);
                bool space_separator_found = (std::string::npos != space_separator_index);
                if (!space_separator_found)
                {
                    return std::nullopt;
                }

                // The material filename starts right after the space separator.
                std::size_t material_filename_start_index = space_separator_index + 1;

                // READ THE MATERIAL FILENAME.
                std::string material_filename = line.substr(material_filename_start_index);
                material_filenames.push_back(material_filename);

                // CONTINUE PROCESSING OTHER LINES.
                continue;
            }

            /// @todo   What to do with usemtl?
            const std::string USE_MATERIAL_KEYWORD = "usemtl";
            bool is_use_material_line = line.starts_with(USE_MATERIAL_KEYWORD);
            if (is_use_material_line)
            {
                continue;
            }

            // CHECK IF AN OBJECT IS BEING DEFINED.
            /// @todo   Should this code handle multiple objects per file?
            constexpr char OBJECT_INDICATOR = 'o';
            bool is_named_object_line = line.starts_with(OBJECT_INDICATOR);
            if (is_named_object_line)
            {
                /// @todo   Track object names?
                continue;
            }

            // CHECK IF VERTEX DATA IS BEING DEFINED.
            constexpr char VERTEX_INDICATOR = 'v';
            bool is_vertex_line = line.starts_with(VERTEX_INDICATOR);
            if (is_vertex_line)
            {
                // CHECK THE TYPE OF VERTEX DATA BEING DEFINED.
                const std::string VERTEX_TEXTURE_COORDINATE_INDICATOR = "vt";
                const std::string VERTEX_NORMAL_INDICATOR = "vn";
                bool is_vertex_texture_coordinates = line.starts_with(VERTEX_TEXTURE_COORDINATE_INDICATOR);
                bool is_vertex_normal_data = line.starts_with(VERTEX_NORMAL_INDICATOR);
                if (is_vertex_texture_coordinates)
                {
                    /// @todo   Handle.  Not important yet.
                }
                else if (is_vertex_normal_data)
                {
                    /// @todo   Handle.  Not important yet.
                }
                else
                {
                    // The line just consists of regular vertex postion data.

                    /// @todo   Make this more efficient.
                    std::istringstream line_data(line);
                    // Skip past the vertex data type indicator.
                    std::string vertex_data_type_indicator;
                    line_data >> vertex_data_type_indicator;

                    MATH::Vector3f vertex_position;
                    line_data >> vertex_position.X;
                    line_data >> vertex_position.Y;
                    line_data >> vertex_position.Z;
                    vertices.push_back(vertex_position);
                }
            }

            /// @todo   What to do with shading line?
            constexpr char SHADING_INDICATOR = 's';
            bool is_shading_line = line.starts_with(SHADING_INDICATOR);
            if (is_shading_line)
            {
                continue;
            }

            /// @todo   What to do with face lines?
            /// They seem to be for quadrilaterals, not triangles.
            constexpr char FACE_INDICATOR = 'f';
            bool is_face_line = line.starts_with(FACE_INDICATOR);
            if (is_face_line)
            {
                continue;
            }
        }

        // LOAD ANY MATERIALS.
        // Materials are expected to all be within the same folder as the .obj file.
        std::filesystem::path model_folder_path = obj_filepath.parent_path();
        std::vector<Material> materials;
        for (const auto& material_filename : material_filenames)
        {
            std::filesystem::path material_filepath = model_folder_path / material_filename;
            std::optional<Material> material = WavefrontMaterial::Load(material_filepath);
            if (material)
            {
                materials.push_back(*material);
            }
        }

        // FORM THE FINAL OBJECT.
        Object3D object_3d;
        
        /// @todo   Need to do triangulation.
        std::size_t current_triangle_vertex_count = 0;
        Triangle current_triangle;
        for (const MATH::Vector3f& vertex : vertices)
        {
            // CHECK IF THE CURRENT TRIANGLE HAS ROOM FOR THE VERTEX.
            bool triangle_has_room_for_vertex = (current_triangle_vertex_count < Triangle::VERTEX_COUNT);
            if (!triangle_has_room_for_vertex)
            {
                // STORE THE CURRENT TRIANGLE IN THE MODEL.
                /// @todo   Need to have way to push final triangle with last vertex.
                object_3d.Triangles.push_back(current_triangle);

                // START A NEW TRIANGLE.
                current_triangle_vertex_count = 0;
            }
            
            // ADD THE VERTEX TO THE TRIANGLE.
            current_triangle.Vertices[current_triangle_vertex_count] = vertex;
            ++current_triangle_vertex_count;

            /// @todo   How to handle materials?  Need some kind of permanent storage.
        }

        return object_3d;
    }
}
