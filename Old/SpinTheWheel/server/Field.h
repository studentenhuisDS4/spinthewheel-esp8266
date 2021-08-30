typedef String (*FieldSetter)(String);
typedef String (*FieldGetter)();

const String NumberFieldType = "Number";
const String BooleanFieldType = "Boolean";
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";
const String SectionFieldType = "Section";

typedef struct Field
{
    String name;
    String label;
    String type;
    uint8_t min;
    uint8_t max;
    FieldGetter getValue;
    FieldGetter getOptions;
    FieldSetter setValue;
};

typedef Field FieldList[];

Field getField(String name, FieldList fields, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        Field field = fields[i];
        if (field.name == name)
        {
            return field;
        }
    }
    return Field();
}

String getFieldValue(String name, FieldList fields, uint8_t count)
{
    Field field = getField(name, fields, count);
    if (field.getValue)
    {
        return field.getValue();
    }
    return String();
}

String setFieldValue(String name, String value, FieldList fields, uint8_t count)
{
    Field field = getField(name, fields, count);
    if (field.setValue)
    {
        return field.setValue(value);
    }
    return String();
}

String getFieldsJson(FieldList fields, uint8_t count)
{
    String json = "[";

    for (uint8_t i = 0; i < count; i++)
    {
        Field field = fields[i];

        json += "{\"name\":\"" + field.name + "\",\"label\":\"" + field.label + "\",\"type\":\"" + field.type + "\"";

        if (field.getValue)
        {
            if (field.type == ColorFieldType || field.type == "String")
            {
                json += ",\"value\":\"" + field.getValue() + "\"";
            }
            else
            {
                json += ",\"value\":" + field.getValue();
            }
        }

        if (field.type == NumberFieldType)
        {
            json += ",\"min\":" + String(field.min);
            json += ",\"max\":" + String(field.max);
        }

        if (field.getOptions)
        {
            json += ",\"options\":[";
            json += field.getOptions();
            json += "]";
        }

        json += "}";

        if (i < count - 1)
            json += ",";
    }

    json += "]";

    return json;
}