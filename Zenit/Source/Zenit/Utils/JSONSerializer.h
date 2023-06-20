#pragma once

#include <parson.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <string>

namespace Zenit {

	struct SerializerObject
	{
		JSON_Object* value = nullptr;
		operator JSON_Object* () { return value; }
		//SerializerObject& operator=(const JSON_Object* other) { return *this; }
	};

	struct SerializerValue
	{
		JSON_Value* value = nullptr;
		operator JSON_Value*() { return value; }
		//SerializerValue& operator=(const JSON_Value* other) { return other; }
	};

	struct SerializerArray
	{
		JSON_Array* value;
		operator JSON_Array* () { return value; }
		//SerializerArray& operator=(const JSON_Array* other) { return *this; }
	};

	class JSONSerializer
	{
	public:
		JSONSerializer();
		virtual ~JSONSerializer();

		static SerializerValue CreateValue();
		static SerializerObject CreateObjectFromNewValue();
		static SerializerObject CreateObjectFromValue(SerializerValue& value);
		static SerializerArray CreateArrayFromNewValue();
		static SerializerValue CreateArrayValue();
		static SerializerArray CreateArrayFromValue(SerializerValue& value);

		static void SetObjectValue(SerializerObject& object, const char* name, SerializerValue& value);

		static void SetNumber(SerializerObject& object, const char* name, float value);
		
		static void SetVector2f(SerializerObject& object, const char* name, glm::vec2 value);
		static void SetVector3f(SerializerObject& object, const char* name, glm::vec3 value);
		static void SetVector4f(SerializerObject& object, const char* name, glm::vec4 value);

		static void SetString(SerializerObject& object, const char* name, const char* value);

		static void DotSetValue(SerializerObject& parent, const char* name, SerializerValue& value);


		static void AppendValueToArray(SerializerArray& array, SerializerValue& value);
		static void AppendNumberToArray(SerializerArray& array, float number);

		static SerializerObject GetObjectWithName(SerializerObject& object, const char* name);
		static SerializerObject GetObjectWithValue(SerializerValue& value);
		static SerializerObject GetObjectFromArray(SerializerArray& array, size_t index);
		static float GetNumberFromArray(SerializerArray& array, size_t index);
		
		static SerializerValue GetValueFromObject(SerializerObject& object, const char* name);
		static float GetNumberFromObject(SerializerObject& object, const char* name);
		static glm::vec2 GetVector2fFromObject(SerializerObject& object, const char* name);
		static glm::vec3 GetVector3fFromObject(SerializerObject& object, const char* name);
		static glm::vec4 GetVector4fFromObject(SerializerObject& object, const char* name);
		static const char* GetStringFromObject(SerializerObject& object, const char* name);
		static SerializerArray GetArrayFromObject(SerializerObject& object, const char* name);
		static size_t GetArraySize(SerializerArray& array);

		static void FreeValue(SerializerValue& value);
		static void DumpFile(SerializerValue& value, const char* pathWithFileName);
		static SerializerValue ReadFile(const char* pathWithFileName);
	};

}