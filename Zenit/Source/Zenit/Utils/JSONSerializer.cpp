#include "JSONSerializer.h"
#include "Zenit/Core/Log.h"

#include <string>

namespace Zenit {

	JSONSerializer::JSONSerializer()
	{
	}

	JSONSerializer::~JSONSerializer()
	{
	}

	SerializerValue JSONSerializer::CreateValue()
	{
		SerializerValue value = SerializerValue();
		value.value = json_value_init_object();
		return value;
	}

	SerializerObject JSONSerializer::CreateObjectFromNewValue()
	{
		SerializerObject object = SerializerObject();
		object.value = json_value_get_object(json_value_init_object());
		return object;
	}

	SerializerObject JSONSerializer::CreateObjectFromValue(SerializerValue& value)
	{
		SerializerObject object = SerializerObject();
		object.value = json_value_get_object(value);
		return object;
	}

	SerializerArray JSONSerializer::CreateArrayFromNewValue()
	{
		SerializerArray array = SerializerArray();
		array.value = json_value_get_array(json_value_init_array());
		return array;
	}

	SerializerValue JSONSerializer::CreateArrayValue()
	{
		SerializerValue value = SerializerValue();
		value.value = json_value_init_array();
		return value;
	}

	SerializerArray JSONSerializer::CreateArrayFromValue(SerializerValue& value)
	{
		SerializerArray array = SerializerArray();
		array.value = json_value_get_array(value);
		return array;
	}

	void JSONSerializer::SetObjectValue(SerializerObject& object, const char* name, SerializerValue& value)
	{
		JSON_Status status = json_object_set_value(object, name, value);
		if (status != JSONSuccess)
		{
			ZN_CORE_ERROR("Set Object Value failed");
		}
	}

	void JSONSerializer::SetNumber(SerializerObject& object, const char* name, int value)
	{
		JSON_Status status = json_object_set_number(object, name, value);
		if (status != JSONSuccess)
		{
			ZN_CORE_ERROR("Set Number failed");
		}
	}

	void JSONSerializer::SetVector2f(SerializerObject& object, const char* name, glm::vec2 value)
	{
		json_object_set_number(object, std::string(name).append(" x").c_str(), value.x);
		json_object_set_number(object, std::string(name).append(" y").c_str(), value.y);
		
	}

	void JSONSerializer::SetVector3f(SerializerObject& object, const char* name, glm::vec3 value)
	{
		json_object_set_number(object, std::string(name).append(" x").c_str(), value.x);
		json_object_set_number(object, std::string(name).append(" y").c_str(), value.y);
		json_object_set_number(object, std::string(name).append(" z").c_str(), value.z);	
	}

	void JSONSerializer::SetVector4f(SerializerObject& object, const char* name, glm::vec4 value)
	{
		json_object_set_number(object, std::string(name).append(" x").c_str(), value.x);
		json_object_set_number(object, std::string(name).append(" y").c_str(), value.y);
		json_object_set_number(object, std::string(name).append(" z").c_str(), value.z);
		json_object_set_number(object, std::string(name).append(" w").c_str(), value.w);
	}

	void JSONSerializer::SetString(SerializerObject& object, const char* name, const char* value)
	{
		JSON_Status status = json_object_set_string(object, name, value);
		if (status != JSONSuccess)
		{
			ZN_CORE_ERROR("Set String failed");
		}
	}

	void JSONSerializer::DotSetValue(SerializerObject& parent, const char* name, SerializerValue& value)
	{
		JSON_Status status = json_object_dotset_value(parent, name, value);
		if (status != JSONSuccess)
		{
			ZN_CORE_ERROR("Dot Set Value failed");
		}
	}

	void JSONSerializer::AppendValueToArray(SerializerArray& array, SerializerValue& value)
	{
		JSON_Status status = json_array_append_value(array, value);
		if (status != JSONSuccess)
		{
			ZN_CORE_ERROR("Appending to Array failed");
		}
	}

	SerializerObject JSONSerializer::GetObjectWithName(SerializerObject& object, const char* name)
	{
		SerializerObject ret = SerializerObject();
		ret.value = json_object_get_object(object, name);
		return ret;
	}
	
	SerializerObject JSONSerializer::GetObjectWithValue(SerializerValue& value)
	{
		SerializerObject ret = SerializerObject();
		ret.value = json_value_get_object(value);
		return ret;
	}

	void JSONSerializer::FreeValue(SerializerValue& value)
	{
		json_value_free(value);
	}

	void JSONSerializer::DumpFile(SerializerValue& value, const char* pathWithFileName)
	{
		JSON_Status status = json_serialize_to_file_pretty(value, pathWithFileName);

		if (status != JSONSuccess)
		{
			ZN_CORE_ERROR("Saving File failed");
		}
	}




}