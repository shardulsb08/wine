/*
 * Copyright (C) 2012 Alistair Leslie-Hughes
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#define COBJMACROS
#include <stdio.h>

#include "windows.h"
#include "ole2.h"
#include "oleauto.h"
#include "dispex.h"

#include "wine/test.h"

#include "scrrun.h"

static void test_interfaces(void)
{
    static const WCHAR key_add[] = {'a', 0};
    static const WCHAR key_add_value[] = {'a', 0};
    static const WCHAR key_non_exist[] = {'b', 0};
    HRESULT hr;
    IDispatch *disp;
    IDispatchEx *dispex;
    IDictionary *dict;
    IObjectWithSite *site;
    VARIANT key, value;
    VARIANT_BOOL exists;
    LONG count = 0;

    hr = CoCreateInstance(&CLSID_Dictionary, NULL, CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER,
            &IID_IDispatch, (void**)&disp);
    ok(hr == S_OK, "got 0x%08x\n", hr);

    VariantInit(&key);
    VariantInit(&value);

    hr = IDispatch_QueryInterface(disp, &IID_IDictionary, (void**)&dict);
    ok(hr == S_OK, "got 0x%08x, expected 0x%08x\n", hr, S_OK);

    hr = IDispatch_QueryInterface(disp, &IID_IObjectWithSite, (void**)&site);
    ok(hr == E_NOINTERFACE, "got 0x%08x, expected 0x%08x\n", hr, E_NOINTERFACE);

    hr = IDispatch_QueryInterface(disp, &IID_IDispatchEx, (void**)&dispex);
    ok(hr == E_NOINTERFACE, "got 0x%08x, expected 0x%08x\n", hr, E_NOINTERFACE);

    V_VT(&key) = VT_BSTR;
    V_BSTR(&key) = SysAllocString(key_add);
    V_VT(&value) = VT_BSTR;
    V_BSTR(&value) = SysAllocString(key_add_value);
    hr = IDictionary_Add(dict, &key, &value);
    todo_wine ok(hr == S_OK, "got 0x%08x, expected 0x%08x\n", hr, S_OK);
    VariantClear(&value);

    exists = VARIANT_FALSE;
    hr = IDictionary_Exists(dict, &key, &exists);
    todo_wine ok(hr == S_OK, "got 0x%08x, expected 0x%08x\n", hr, S_OK);
    todo_wine ok(exists == VARIANT_TRUE, "Expected TRUE but got FALSE.\n");
    VariantClear(&key);

    exists = VARIANT_TRUE;
    V_VT(&key) = VT_BSTR;
    V_BSTR(&key) = SysAllocString(key_non_exist);
    hr = IDictionary_Exists(dict, &key, &exists);
    todo_wine ok(hr == S_OK, "got 0x%08x, expected 0x%08x\n", hr, S_OK);
    todo_wine ok(exists == VARIANT_FALSE, "Expected FALSE but got TRUE.\n");
    VariantClear(&key);

    hr = IDictionary_get_Count(dict, &count);
    ok(hr == S_OK, "got 0x%08x, expected 0x%08x\n", hr, S_OK);
    todo_wine ok(count == 1, "got %d, expected 1\n", count);

    IDictionary_Release(dict);
    IDispatch_Release(disp);
}

static void test_comparemode(void)
{
    CompareMethod method;
    IDictionary *dict;
    HRESULT hr;

    hr = CoCreateInstance(&CLSID_Dictionary, NULL, CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER,
            &IID_IDictionary, (void**)&dict);
    ok(hr == S_OK, "got 0x%08x\n", hr);

if (0) /* crashes on native */
    hr = IDictionary_get_CompareMode(dict, NULL);

    method = 10;
    hr = IDictionary_get_CompareMode(dict, &method);
    ok(hr == S_OK, "got 0x%08x\n", hr);
    ok(method == BinaryCompare, "got %d\n", method);

    /* invalid mode value is not checked */
    hr = IDictionary_put_CompareMode(dict, 10);
    ok(hr == S_OK, "got 0x%08x\n", hr);

    hr = IDictionary_get_CompareMode(dict, &method);
    ok(hr == S_OK, "got 0x%08x\n", hr);
    ok(method == 10, "got %d\n", method);

    hr = IDictionary_put_CompareMode(dict, DatabaseCompare);
    ok(hr == S_OK, "got 0x%08x\n", hr);

    hr = IDictionary_get_CompareMode(dict, &method);
    ok(hr == S_OK, "got 0x%08x\n", hr);
    ok(method == DatabaseCompare, "got %d\n", method);

    IDictionary_Release(dict);
}

START_TEST(dictionary)
{
    IDispatch *disp;
    HRESULT hr;

    CoInitialize(NULL);

    hr = CoCreateInstance(&CLSID_Dictionary, NULL, CLSCTX_INPROC_SERVER|CLSCTX_INPROC_HANDLER,
            &IID_IDispatch, (void**)&disp);
    if(FAILED(hr)) {
        win_skip("Dictionary object is not supported: %08x\n", hr);
        CoUninitialize();
        return;
    }
    IDispatch_Release(disp);

    test_interfaces();
    test_comparemode();

    CoUninitialize();
}
