/*
 *                         Vortex OpenSplice
 *
 *   This software and documentation are Copyright 2006 to TO_YEAR ADLINK
 *   Technology Limited, its affiliated companies and licensors. All rights
 *   reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */
#include "idl_genCxxHelper.h"
#include "idl_genLanguageHelper.h"
#include "os_heap.h"
#include "os_stdlib.h"
#include "os_abstract.h"
#include "c_misc.h"
#include "c_metabase.h"
#include "ut_collection.h"

/* Specify a list of all C++ keywords */
static const char *cxx_keywords[74] = {
    /* QAC EXPECT 5007; Bypass qactools error */
    "and", "and_eq", "asm", "auto", "bitand", "bitor",
    "bool", "break", "case", "catch", "char", "class",
    "compl", "const", "const_cast", "continue", "default", "delete",
    "do", "double", "dynamic_cast", "else", "enum", "explicit",
    "export", "extern", "false", "float", "for", "friend",
    "goto", "if", "inline", "int", "long", "mutable",
    "namespace", "new", "not", "not_eq", "operator", "or",
    "or_eq", "private", "protected", "public", "register", "reinterpret_cast",
    "return", "short", "signed", "sizeof", "static", "static_cast",
    "struct", "switch", "template", "this", "throw", "true",
    "try", "typedef", "typeid", "typename", "union", "unsigned",
    "using", "virtual", "void", "volatile", "wchar_t", "while",
    "xor", "xor_eq"
};

/* Translate an IDL identifier into  a C++ language identifier.
   The IDL specification states that all identifiers that match
   a C++ keyword must be prepended by "_cxx_".
*/
c_char *
idl_cxxId(
    const char *identifier)
{
    size_t i;
    char *cxxId;

    /* search through the C++ keyword list */
    /* QAC EXPECT 5003; Bypass qactools error, why is this a violation */
    for (i = 0; i < sizeof(cxx_keywords)/sizeof(c_char *); i++) {
	/* QAC EXPECT 5007, 3416; will not use wrapper, no side effects here */
	if (strcmp(cxx_keywords[i], identifier) == 0) {
	    /* If a keyword matches the specified identifier, prepend _cxx_ */
	    /* QAC EXPECT 5007; will not use wrapper */
	    cxxId = os_malloc(strlen(identifier)+1+5);
	    snprintf(cxxId, strlen(identifier)+1+5, "_cxx_%s", identifier);
	    return cxxId;
	}
    }
    /* No match with a keyword is found, thus return the identifier itself */
    cxxId = os_strdup(identifier);
    return cxxId;
    /* QAC EXPECT 2006; performance is selected above rules here */
}

/* Build a textual presenation of the provided scope stack taking the
   C++ keyword identifier translation into account. Further the function
   equals "idl_scopeStack".
*/
c_char *
idl_scopeStackCxx(
    idl_scope scope,
    const char *scopeSepp,
    const char *name)
{
    c_long si;
    c_long sz;
    c_char *scopeStack;
    c_char *Id;

    si = 0;
    sz = idl_scopeStackSize(scope);
    if (si < sz) {
        /* The scope stack is not empty */
        /* Copy the first scope element name */
        scopeStack = os_strdup(scopeSepp);/* start with the seperator */
        Id = idl_cxxId(idl_scopeElementName(idl_scopeIndexed(scope, si)));
        scopeStack = os_realloc(scopeStack, strlen(scopeStack)+strlen(scopeSepp)+strlen(Id)+1);
        os_strcat(scopeStack, Id);
        si++;
        while (si < sz) {
            /* Translate the scope name to a C++ identifier */
            Id = idl_cxxId(idl_scopeElementName(idl_scopeIndexed(scope, si)));
            /* allocate space for the current scope stack + the separator
               and the next scope name
             */
            /* QAC EXPECT 5007; will not use wrapper */
            scopeStack = os_realloc(scopeStack, strlen(scopeStack)+strlen(scopeSepp)+strlen(Id)+1);
           /* Concatenate the separator */
           /* QAC EXPECT 5007; will not use wrapper */
           os_strcat(scopeStack, scopeSepp);
           /* Concatenate the scope name */
           /* QAC EXPECT 5007; will not use wrapper */
           os_strcat(scopeStack, Id);
           si++;
        }
        if (name) {
            /* A user identifier is specified */
            /* Translate the user identifier to a C++ identifier */
            Id = idl_cxxId(name);
            /* allocate space for the current scope stack + the separator
               and the user identifier
             */
            /* QAC EXPECT 5007; will not use wrapper */
            scopeStack = os_realloc(scopeStack, strlen(scopeStack)+strlen(scopeSepp)+strlen(Id)+1);
            /* Concatenate the separator */
            /* QAC EXPECT 5007; will not use wrapper */
            os_strcat(scopeStack, scopeSepp);
            /* Concatenate the user identifier */
            /* QAC EXPECT 5007; will not use wrapper */
            os_strcat(scopeStack, Id);
        }
    } else {
	/* The stack is empty */
	if (name) {
	    /* A user identifier is specified */
	    scopeStack = os_strdup(idl_cxxId(name));
	} else {
	    /* make the stack represenation empty */
	    scopeStack = os_strdup("");
	}
    }
    /* return the scope stack representation */
    return scopeStack;
}

/* Return the C++ specific type identifier for the
   specified type specification
*/
c_char *
idl_corbaCxxTypeFromTypeSpec(
        idl_typeSpec typeSpec)
{
    c_type t = idl_typeSpecDef(typeSpec);
    return idl_CxxTypeFromCType(t, "<NotApplicable>");
}

static c_char *
CxxFromfullyScopedName(c_char *fullyScopedName)
{
    c_char *savePtr = NULL;
    c_char *name, *cxxName, *typeName = NULL;
    size_t totalLen = strlen(fullyScopedName) + 2 + 1;
    size_t len = 0;

    name = os_strtok_r(fullyScopedName, ":", &savePtr);
    if (name) {
        cxxName = idl_cxxId(name);
        if (strlen(cxxName) != strlen(name)) {
            totalLen += (strlen(cxxName) - strlen(name));
        }
        typeName = os_malloc(totalLen);

        len = (size_t)snprintf(typeName, totalLen, "::%s", cxxName);
        assert(len < totalLen);
        os_free(cxxName);
        name = os_strtok_r(NULL, ":", &savePtr);
        while (name != NULL) {
            cxxName = idl_cxxId(name);
            if (strlen(cxxName) != strlen(name)) {
                totalLen += (strlen(cxxName) - strlen(name));
                typeName = os_realloc(typeName, totalLen);
            }
            assert(typeName[len] == '\0');
            len += (size_t)snprintf(typeName + len, totalLen - len, "::%s", cxxName);
            assert(len < totalLen);
            os_free(cxxName);
            name = os_strtok_r(NULL, ":", &savePtr);
        }
    } else {
        typeName = os_malloc(1);
        typeName[0] = '\0';
    }

    return typeName;
}

c_char *
idl_CxxTypeFromCType(
    c_type t,
    const char *memberName)
{
    c_char *typeName = NULL;
    c_char *subTypeName;
    const c_char *template;
    size_t typeNameSize;

    switch (c_baseObjectKind(t)) {
    case M_PRIMITIVE:
        switch (c_primitiveKind(t))
        {
        case P_SHORT:
            typeName = os_strdup("::DDS::Short");
            break;
        case P_USHORT:
            typeName = os_strdup("::DDS::UShort");
            break;
        case P_LONG:
            typeName = os_strdup("::DDS::Long");
            break;
        case P_ULONG:
            typeName = os_strdup("::DDS::ULong");
            break;
        case P_LONGLONG:
            typeName = os_strdup("::DDS::LongLong");
            break;
        case P_ULONGLONG:
            typeName = os_strdup("::DDS::ULongLong");
            break;
        case P_FLOAT:
            typeName = os_strdup("::DDS::Float");
            break;
        case P_DOUBLE:
            typeName = os_strdup("::DDS::Double");
            break;
        case P_CHAR:
            typeName = os_strdup("::DDS::Char");
            break;
        case P_BOOLEAN:
            typeName = os_strdup("::DDS::Boolean");
            break;
        case P_OCTET:
            typeName = os_strdup("::DDS::Octet");
            break;
        default:
            assert(0);
            break;
        }
        break;
    case M_COLLECTION:
        switch (c_collectionTypeKind(t))
        {
        case OSPL_C_STRING:
            typeName = os_strdup("::DDS::String_mgr");
            break;
        case OSPL_C_SEQUENCE:
            template = "_%s_seq";
            typeNameSize = strlen(template) + strlen(memberName) + 1;
            typeName = os_malloc(typeNameSize);
            (void)snprintf(typeName, typeNameSize, template, memberName);
            break;
        case OSPL_C_ARRAY:
            template = "_%s";
            typeNameSize = strlen(template) + strlen(memberName) + 1;
            typeName = os_malloc(typeNameSize);
            (void)snprintf(typeName, typeNameSize, template, memberName);
            break;
        default:
            assert(0);
            break;
        }
        break;
    case M_ENUMERATION:
    case M_STRUCTURE:
    case M_UNION:
    case M_TYPEDEF:
        subTypeName = c_metaScopedName(c_metaObject(t));
        typeName = CxxFromfullyScopedName(subTypeName);
        os_free(subTypeName);
        break;
    default:
        assert(0);
        break;
    }
    return typeName;
}

c_char *
idl_CxxTypeFromTypeSpec(
        idl_typeSpec typeSpec)
{
    c_type t = idl_typeSpecDef(typeSpec);
    return idl_CxxTypeFromCType(t, "<NotApplicable>");
}

static c_char *
abstractEnumLabel(c_type t, c_ulong i)
{
    c_char *scopeName, *cxxScopeName, *cxxEnumLabel, *abstractLabel;
    const c_char *enumTmplt = "%s::%s";
    size_t abstractLabelLen;

    scopeName = c_metaScopedName(c_metaObject(t)->definedIn);
    cxxScopeName = CxxFromfullyScopedName(scopeName);
    cxxEnumLabel = idl_cxxId(c_metaObject(c_enumeration(t)->elements[i])->name);
    abstractLabelLen = strlen(enumTmplt) + strlen(cxxScopeName) + strlen(cxxEnumLabel) + 1;
    abstractLabel = os_malloc(abstractLabelLen);
    (void)snprintf(abstractLabel, abstractLabelLen, enumTmplt, cxxScopeName, cxxEnumLabel);
    os_free(cxxEnumLabel);
    os_free(cxxScopeName);
    os_free(scopeName);
    return abstractLabel;
}

c_char *
idl_CxxDefaultValueFromCType(
    c_type t)
{
    os_char *defValue = NULL;

    t = c_typeActualType(t);
    switch (c_baseObjectKind(t)) {
    case M_PRIMITIVE:
        switch (c_primitiveKind(t))
        {
        case P_SHORT:
        case P_USHORT:
        case P_LONG:
        case P_ULONG:
        case P_LONGLONG:
        case P_ULONGLONG:
        case P_CHAR:
        case P_OCTET:
            defValue = os_strdup("0");
            break;
        case P_FLOAT:
            defValue = os_strdup("0.0f");
            break;
        case P_DOUBLE:
            defValue = os_strdup("0.0");
            break;
        case P_BOOLEAN:
            defValue = os_strdup("false");
            break;
        default:
            assert(0);
            break;
        }
        break;
    case M_ENUMERATION:
        defValue = abstractEnumLabel(t, 0);
        break;
    case M_COLLECTION:
    case M_STRUCTURE:
    case M_UNION:
        defValue = NULL; /* These classes have their own default constructor. */
        break;
    case M_TYPEDEF:
        assert(0); /* Typedef has already been resolved: should not get here! */
        break;
    default:
        assert(0);
        break;
    }
    return defValue;
}

c_char *
idl_CxxValueFromCValue(
    c_type t,
    c_value v)
{
    os_char *cxxValue = NULL;

    t = c_typeActualType(t);
    switch (c_baseObjectKind(t)) {
    case M_PRIMITIVE:
        switch (c_primitiveKind(t))
        {
        case P_LONG:
            cxxValue = os_malloc(40);
            if (v.is.Long != INT32_MIN) {
                snprintf(cxxValue, 40, "%dL", v.is.Long);
            } else {
                snprintf(cxxValue, 40, "(-2147483647L - 1L)");
            }
            break;
        case P_ULONG:
            cxxValue = os_malloc(40);
            snprintf(cxxValue, 40, "%uU", v.is.ULong);
            break;
        case P_LONGLONG:
            cxxValue = os_malloc(40);
            if (v.is.LongLong != INT64_MIN) {
                snprintf(cxxValue, 40, "%"PA_PRId64"LL", v.is.LongLong);
            } else {
                snprintf(cxxValue, 40, "(-9223372036854775807LL - 1LL)");
            }
            break;
        case P_ULONGLONG:
            cxxValue = os_malloc(40);
            snprintf(cxxValue, 40, "%"PA_PRIu64"ULL", v.is.ULongLong);
            break;
        case P_OCTET:
        case P_FLOAT:
        case P_DOUBLE:
        case P_SHORT:
        case P_USHORT:
            cxxValue = c_valueImage(v);
            break;
        case P_BOOLEAN:
            if (v.is.Boolean) {
                cxxValue = os_strdup("TRUE");
            } else {
                cxxValue = os_strdup("FALSE");
            }
            break;
        case P_CHAR:
            if (v.is.Char < 32) {
                cxxValue = os_malloc(5); /* sign, 3 digits and '\0' */
                (void)snprintf(cxxValue, 5, "%d", v.is.Char);
            } else {
                char *charValue = c_valueImage(v);
                cxxValue = os_malloc(4);
                (void)snprintf(cxxValue, 4, "'%s'", charValue);
                os_free(charValue);
            }
            break;
        default:
            assert(0);
            break;
        }
        break;
    case M_ENUMERATION:
        cxxValue = abstractEnumLabel(t, (c_ulong) v.is.Long);
        break;
    case M_COLLECTION:
    case M_STRUCTURE:
    case M_UNION:
    case M_TYPEDEF:
        assert(0); /* None of these types should be expected here */
        break;
    default:
        assert(0);
        break;
    }
    return cxxValue;
}

c_bool
idl_CxxIsRefType(
    c_type t)
{
    t = c_typeActualType(t);

    switch (c_baseObjectKind(t)) {
    case M_PRIMITIVE:
    case M_ENUMERATION:
        return FALSE;
    case M_COLLECTION:
        if (c_collectionTypeKind(t) == OSPL_C_STRING) {
            return TRUE;
        } else if (c_collectionTypeKind(t) == OSPL_C_SEQUENCE) {
            return TRUE;
        } else if (c_collectionTypeKind(t) == OSPL_C_ARRAY) {
            return idl_CxxIsRefType(c_collectionTypeSubType(t));
        } else {
            assert(0);
            break;
        }
    case M_STRUCTURE:
        return (c_structure(t)->references != NULL);
    case M_UNION:
        return (c_union(t)->references != NULL);
    case M_TYPEDEF:
        assert(0); /* Typedef should have been resolved already. */
        break;
    default:
        assert(0);
        break;
    }
    return FALSE;
}

c_char *
idl_CxxInTypeFromCType(
    c_type t,
    const char *memberName)
{
    os_char *inType = NULL;
    os_char *paramType;
    const char *template;
    size_t size;
    c_type actualType;

    switch (c_baseObjectKind(t)) {
    case M_PRIMITIVE:
    case M_ENUMERATION:
        inType = idl_CxxTypeFromCType(t, memberName);
        break;
    case M_COLLECTION:
        if (c_collectionTypeKind(t) == OSPL_C_ARRAY) {
            template = "%s_slice*";
            paramType = idl_CxxTypeFromCType(t, memberName);
            size = strlen(template) + strlen(paramType) + 1;
            inType = os_malloc(size);
            (void)snprintf(inType, size, template, paramType);
            os_free(paramType);
            break;
        } else if (c_collectionTypeKind(t) == OSPL_C_STRING) {
            inType = os_strdup("char*");
            break;
        }
        /* For sequence types, the fall-through is intentional. */
    case M_STRUCTURE:
    case M_UNION:
        template = "const %s&";
        paramType = idl_CxxTypeFromCType(t, memberName);
        size = strlen(template) + strlen(paramType) + 1;
        inType = os_malloc(size);
        (void)snprintf(inType, size, template, paramType);
        os_free(paramType);
        break;
    case M_TYPEDEF:
        actualType = c_typeActualType(t);
        switch(c_baseObjectKind(actualType)) {
        case M_PRIMITIVE:
        case M_ENUMERATION:
            inType = idl_CxxTypeFromCType(t, memberName);
            break;
        case M_COLLECTION:
            if (c_collectionTypeKind(actualType) == OSPL_C_STRING) {
                inType = os_strdup("char*");
                break;
            } else if (c_collectionTypeKind(actualType) == OSPL_C_ARRAY) {
                template = "%s_slice*";
                paramType = idl_CxxTypeFromCType(t, memberName);
                size = strlen(template) + strlen(paramType) + 1;
                inType = os_malloc(size);
                (void)snprintf(inType, size, template, paramType);
                os_free(paramType);
                break;
            }
            /* For sequence types, the fall-through is intentional. */
        case M_STRUCTURE:
        case M_UNION:
            template = "const %s&";
            paramType = idl_CxxTypeFromCType(t, memberName);
            size = strlen(template) + strlen(paramType) + 1;
            inType = os_malloc(size);
            (void)snprintf(inType, size, template, paramType);
            os_free(paramType);
            break;
        default:
            assert(0);
            break;
        }
        break;
    default:
        assert(0);
        break;
    }
    return inType;
}

c_char *
idl_genCxxConstantGetter(void)
{
    return NULL;
}

const c_char*
idl_isocppCxxStructMemberSuffix()
{
  return idl_getIsISOCpp() && idl_getIsISOCppTypes() ? "_" : "";
}

static os_equality
orderLabels (
    c_object o1,
    c_object o2,
    c_voidp args)
{
    c_value *v1 = (c_value *)o1;
    c_value *v2 = (c_value *)o2;

    OS_UNUSED_ARG(args);

    /* The cast below is safe due to the static compile-time assert above. */
    return (os_equality) c_valueCompare(*v1, *v2);
}

/* This compile-time constraint assures that the cast from c_equality to
 * os_equality is allowed. */
struct os_equality_equals_c_equality_constraint {
    char require_value_OS_LT_eq_value_C_LT [OS_LT == (os_equality)C_LT];
    char require_value_OS_EQ_eq_value_C_EQ [OS_EQ == (os_equality)C_EQ];
    char require_value_OS_GT_eq_value_C_GT [OS_GT == (os_equality)C_GT];
    char require_value_OS_NE_eq_value_C_NE [OS_NE == (os_equality)C_NE];
    char non_empty_dummy_last_member[1];
};

static c_value
typeMinValue(c_type t)
{
    switch(c_baseObjectKind(t)) {
    case M_PRIMITIVE:
        switch (c_primitiveKind(t))
        {
        case P_SHORT:
            return c_shortMinValue();
        case P_USHORT:
            return c_ushortMinValue();
        case P_LONG:
            return c_longMinValue();
        case P_ULONG:
            return c_ulongMinValue();
        case P_LONGLONG:
            return c_longlongMinValue();
        case P_ULONGLONG:
            return c_ulonglongMinValue();
        case P_OCTET:
            return c_octetMinValue();
        case P_BOOLEAN:
            return c_boolMinValue();
        case P_CHAR:
            return c_charMinValue();
        default:
            assert(0);
            return c_undefinedValue();
        }
        break;
    case M_ENUMERATION:
        return c_longValue(0);
    default:
        assert(0);
        return c_undefinedValue();
    }
}

static c_value
typeOneValue(c_type t)
{
    switch(c_baseObjectKind(t)) {
    case M_PRIMITIVE:
        switch (c_primitiveKind(t))
        {
        case P_SHORT:
            return c_shortValue(1);
        case P_USHORT:
            return c_ushortValue(1);
        case P_LONG:
            return c_longValue(1);
        case P_ULONG:
            return c_ulongValue(1);
        case P_LONGLONG:
            return c_longlongValue(1);
        case P_ULONGLONG:
            return c_ulonglongValue(1);
        case P_OCTET:
            return c_octetValue(1);
        case P_BOOLEAN:
            return c_boolValue(1);
        case P_CHAR:
            return c_charValue(1);
        default:
            assert(0);
            return c_undefinedValue();
        }
        break;
    case M_ENUMERATION:
        return c_longValue(1);
    default:
        assert(0);
        return c_undefinedValue();
    }
}

static c_value
typeMaxValue(c_type t)
{
    switch(c_baseObjectKind(t)) {
    case M_PRIMITIVE:
        switch (c_primitiveKind(t))
        {
        case P_SHORT:
            return c_shortMaxValue();
        case P_USHORT:
            return c_ushortMaxValue();
        case P_LONG:
            return c_longMaxValue();
        case P_ULONG:
            return c_ulongMaxValue();
        case P_LONGLONG:
            return c_longlongMaxValue();
        case P_ULONGLONG:
            return c_ulonglongMaxValue();
        case P_OCTET:
            return c_octetMaxValue();
        case P_BOOLEAN:
            return c_boolMaxValue();
        case P_CHAR:
            return c_charMaxValue();
        default:
            return c_undefinedValue();
            break;
        }
        break;
    case M_ENUMERATION:
        return c_longValue((c_long) (c_enumerationCount(t) - 1));
    default:
        assert(0);
        return c_undefinedValue();
    }
}

c_value
idl_CxxLowestUnionDefaultValue(
        c_type t)
{
    c_value lowestValue, highestValue, unitValue;
    c_ulong i, j, nrBranches;
    ut_table labelValues;
    c_value *label;
    c_equality indexVsBound;
    c_type discrType = c_typeActualType(c_unionUnionSwitchType(t));

    /* First populate a binary tree with all available union labels. */
    nrBranches = c_unionUnionCaseCount(t);
    labelValues = ut_tableNew(orderLabels, NULL, NULL, NULL, NULL, NULL);
    for (i = 0; i < nrBranches; i++) {
        c_unionCase branch = c_unionUnionCase(t, i);
        c_ulong nrLabels = c_arraySize(branch->labels);
        for (j = 0; j < nrLabels; j++) {
            label = &(c_literal(branch->labels[j])->value);
            (void) ut_tableInsert(labelValues, label, label);
        }
    }

    /* Now start looking for the lowest value that is still available. */
    highestValue = typeMaxValue(discrType);
    unitValue = typeOneValue(discrType);
    lowestValue = typeMinValue(discrType);
    indexVsBound = c_valueCompare(lowestValue, highestValue);
    while (indexVsBound == C_LT || indexVsBound == C_EQ) {
        if (!ut_get((ut_collection) labelValues, &lowestValue)) {
            ut_tableFree(labelValues);
            return lowestValue;
        }
        lowestValue = c_valueCalculate(lowestValue, unitValue, O_ADD);
        indexVsBound = c_valueCompare(lowestValue, highestValue);
    }
    ut_tableFree(labelValues);
    return c_undefinedValue();
}
