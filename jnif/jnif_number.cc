
#include "jnif.h"


static jmethodID m_eobject__testNumber;
static jmethodID m_enumber__doubleValue;
static jmethodID m_enumber__intValue;
static jmethodID m_enumber__longValue;

static jclass    ERT_class;
static jmethodID m_ERT__box_int;
static jmethodID m_ERT__box_long;
static jmethodID m_ERT__box_double;
static jmethodID m_ERT__box_parse;

static uint32_t INT_MAX_VALUE;
static uint64_t LONG_MAX_VALUE;



void initialize_jnif_number(JavaVM* vm, JNIEnv *je)
{
  jclass eobject_class      = je->FindClass("erjang/EObject");
  m_eobject__testNumber     = je->GetMethodID(eobject_class,
                                              "testNumber",
                                              "()Lerjang/ENumber;");

  jclass enumber_class      = je->FindClass("erjang/ENumber");
  m_enumber__doubleValue    = je->GetMethodID(enumber_class,
                                              "doubleValue",
                                              "()D");
  m_enumber__intValue       = je->GetMethodID(enumber_class,
                                              "intValue",
                                              "()I");
  m_enumber__longValue      = je->GetMethodID(enumber_class,
                                              "longValue",
                                              "()J");

  ERT_class = (jclass) je->NewGlobalRef ( je->FindClass("erjang/ERT") );
  m_ERT__box_int = je->GetStaticMethodID
    (ERT_class, "box", "(I)Lerjang/ESmall;");
  m_ERT__box_long = je->GetStaticMethodID
    (ERT_class, "box", "(J)Lerjang/EInteger;");
  m_ERT__box_double = je->GetStaticMethodID
    (ERT_class, "box", "(D)Lerjang/EDouble;");
  m_ERT__box_parse = je->GetStaticMethodID
    (ERT_class, "box_parse", "(Ljava/lang/String;)Lerjang/EInteger;");

  jclass Integer_class = je->FindClass("java/lang/Integer");
  jfieldID f_int_max_value = je->GetStaticFieldID(Integer_class, "MAX_VALUE", "I");
  INT_MAX_VALUE = je->GetStaticIntField(Integer_class, f_int_max_value);

  jclass Long_class = je->FindClass("java/lang/Long");
  jfieldID f_long_max_value = je->GetStaticFieldID(Long_class, "MAX_VALUE", "J");
  LONG_MAX_VALUE = je->GetStaticLongField(Long_class, f_long_max_value);

}


int enif_get_double(ErlNifEnv* ee, ERL_NIF_TERM term, double* dp)
{
  jobject o = ee->je->CallObjectMethod(E2J(term), m_eobject__testNumber);
  if (o == NULL)
    return NIF_FALSE;

  *dp = ee->je->CallDoubleMethod(o, m_enumber__doubleValue);
  return NIF_TRUE;
}

int enif_get_int(ErlNifEnv* ee, ERL_NIF_TERM term, int* ip)
{
  jobject o = ee->je->CallObjectMethod(E2J(term), m_eobject__testNumber);
  if (o == NULL)
    return NIF_FALSE;

  *ip = ee->je->CallIntMethod(o, m_enumber__intValue);
  return NIF_TRUE;
}

int enif_get_int64(ErlNifEnv* ee, ERL_NIF_TERM term, ErlNifSInt64* ip)
{
  jobject o = ee->je->CallObjectMethod(E2J(term), m_eobject__testNumber);
  if (o == NULL)
    return NIF_FALSE;

  *ip = (jlong) ee->je->CallLongMethod(o, m_enumber__longValue);
  return NIF_TRUE;
}

int enif_get_uint(ErlNifEnv* ee, ERL_NIF_TERM term, unsigned* ip)
{
  jobject o = ee->je->CallObjectMethod(E2J(term), m_eobject__testNumber);
  if (o == NULL)
    return NIF_FALSE;

  jlong value = ee->je->CallLongMethod(o, m_enumber__longValue);
  if (value < 0)
    return NIF_FALSE;

  *ip = (jlong) value;
  return NIF_TRUE;
}

extern ERL_NIF_TERM enif_make_uint (ErlNifEnv* ee, unsigned i)
{
  jobject boxed;

  if (i > INT_MAX_VALUE) {
    boxed = ee->je->CallStaticObjectMethod(ERT_class, m_ERT__box_long, (jlong) i);
  } else {
    boxed = ee->je->CallStaticObjectMethod(ERT_class, m_ERT__box_int, (jint) i);
  }

  return jnif_retain(ee, boxed);
}

extern ERL_NIF_TERM enif_make_int (ErlNifEnv* ee, int i)
{
  jobject boxed;
  boxed = ee->je->CallStaticObjectMethod(ERT_class, m_ERT__box_int, (jint) i);
  return jnif_retain(ee, boxed);
}

extern ERL_NIF_TERM enif_make_ulong (ErlNifEnv* ee, unsigned long i)
{
  jobject boxed;

  if (i > LONG_MAX_VALUE) {
    const int n = snprintf(NULL, 0, "%lu", i);
    assert(n > 0);
    char buf[n+1];
    int c = snprintf(buf, n+1, "%lu", i);
    assert(buf[n] == '\0');

    jobject str = ee->je->NewStringUTF(buf);

    boxed = ee->je->CallStaticObjectMethod(ERT_class, m_ERT__box_parse, buf);
  } else {
    boxed = ee->je->CallStaticObjectMethod(ERT_class, m_ERT__box_long, (jlong) i);
  }

  return jnif_retain(ee, boxed);
}