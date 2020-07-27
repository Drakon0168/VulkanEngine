{
    "Name": "Debug Manager",
    "Description": "The Debug Manager is a manager class that handles Vulkan's validation layer errors. Vulkan uses validation layers to display errors, through the use of a Debug Utils Messenger, this class takes those errors and displays them to the console window.",
    "Variables": [
        {
            "Name": "instance",
            "Type": "DebugManager*",
            "Description": "This is a pointer to the static instance of the Debug Manager class for the singleton design pattern."
        },
        {
            "Name": "debugMessenger",
            "Type": "VkDebugUtilsMessengerEXT*",
            "Description": "This is the Debug Utils Messenger used by Vulkan to display errors."
        },
        {
            "Name": "validationLayers",
            "Type": "const std::vector<const char*>",
            "Description": "This is the list of validation layers to enable. A lot of the errors we will run into are not fatal and as such can be ignored without crashing the program. This is where we tell Vulkan which errors should be ignored and which ones should be displayed with the debug messenger."
        },
        {
            "Name": "enableValidationLayers",
            "Type": "const bool",
            "Description": "This boolean decides whether or not we want to actually use validation layers. We set this with the '#ifdef NDEBUG' line, this tells the compiler to ignore the line that sets it to true if the compiler is set to compile in release mode otherwise ignore the line that sets it to false. This way we don't use validation layers if the application is running in release mode."
        }
    ],
    "Methods": [
        {
            "Name": "GetInstance",
            "Description": "Returns the static instance of the Debug Manager class for the singleton design pattern.",
            "Parameters": [
            ],
            "Returns": {
                "Type": "static DebugManager*",
                "Description": "The static instance of the Debug Manager class for the singleton design pattern."
            }
        },
        {
            "Name": "Cleanup",
            "Description": "Cleans up the memory that is used by the Debug Utils Messenger.",
            "Parameters": [
            ],
            "Returns": {}
        },
        {
            "Name": "GetEnableValidationLayers",
            "Description": "Returns the enableValidationLayers variable.",
            "Parameters": [
            ],
            "Returns": {
                "Type": "Bool",
                "Description": "Whether or not validation layers are enabled."
            }
        },
        {
            "Name": "GetValidationLayers",
            "Description": "Returns the list of validation layers to be enabled.",
            "Parameters": [
            ],
            "Returns": {
                "Type": "std::vector<const char*>",
                "Description": "The list of validation layers to be enabled."
            }
        },
        {
            "Name": "SetupDebugMessenger",
            "Description": "Creates and allocates the debug messenger that will be used by the Vulkan instance to throw errors.",
            "Parameters": [
            ],
            "Returns": {}
        },
        {
            "Name": "PopulateDebugMessengerCreateInfo",
            "Description": "Takes in a VkDebugUtilsMessengerCreateInfoEXT and fills in the necessary data to create a VkDebugUtilsMessengerEXT.",
            "Parameters": [
                {
                    "Name": "createInfo",
                    "Type": "VkDebugUtilsMessengerCreateInfoEXT",
                    "Description": "This is the create info that will be populated by this method and then used to create the VkDebugUtilsMessengerEXT."
                }
            ],
            "Returns": {}
        },
        {
            "Name": "CheckValidationLayerSupport",
            "Description": "Checks that the computer the application is running on can support all of the validation layers requested.",
            "Parameters": [
            ],
            "Returns": {
                "Type": "Bool",
                "Description": "Returns true if all of the requested validation layers are supported."
            }
        },
        {
            "Name": "DebugCallback",
            "Description": "This method is called whenever a validation layer detects an error and allows us to control how it is displayed. It must return a bool in order to be setup as the debug callback for the instance which is why it returns false.",
            "Parameters": [
            ],
            "Returns": {
                "Type": "Bool",
                "Description": "Returns false."
            }
        }
    ]
}