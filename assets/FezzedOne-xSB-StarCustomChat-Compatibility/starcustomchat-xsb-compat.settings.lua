
-- < END OF BASE SCRIPT > --

local base_init = init
function init()
    local base_root_imageSize = root.imageSize
    -- Needs to be baby-proofed because SCC doesn't sanitise input or handle exceptions from this callback at all. Ugh.
    root.imageSize = function(imagePath)
        if imagePath == "" or imagePath:sub(1, 1) ~= "/" then
            imagePath = "/assetmissing.png" .. imagePath
        end
        local status, result = pcall(base_root_imageSize, imagePath)
        if status then
            return result
        else
            sb.logWarn("[StarCustomChat] Error getting image size: %s", result)
            return {0, 0}
        end
    end

    return base_init()
end