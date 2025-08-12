/**
 * @copyright
 * Copyright (c) 2020 by LG Electronics Inc.
 * This program or software including the accompanying associated documentation
 * ("Software") is the proprietary software of LG Electronics Inc. and or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate written license agreement between you
 * and LG Electronics Inc. ("Authorized License"). Except as set forth in an
 * Authorized License, LG Electronics Inc. grants no license (express or implied),
 * rights to use, or waiver of any kind with respect to the Software, and LG
 * Electronics Inc. expressly reserves all rights in and to the Software and all
 * intellectual property therein. If you have no Authorized License, then you have
 * no rights to use the Software in any ways, and should immediately notify LG
 * Electronics Inc. and discontinue all use of the Software.
 * @file        app_manager_adapter.h
 * @author
 *    copyright (c) 2020  Nguyen Tri Dung <tridung.nguyen@lge.com>
 * @version     1.0.00
*/
#ifndef __OEM_CALL_APP_MANAGER_ADAPTER_H__
#define __OEM_CALL_APP_MANAGER_ADAPTER_H__
#include <memory>
#include <string>
#include <functional>
#include <utils/Message.h>


namespace oc {

/**
 * @brief
 * This is the class which implement method to manager communication request to application manager
 *
 */
class AppManagerAdapter{
public:
    struct FeatureHandler{
        std::string featureName;
        std::string type = "";
        std::function<bool(const std::string type, const std::string featureName)> onStart;
        std::function<bool(const std::string type, const std::string featureName)> onStop;
        std::function<bool(const std::string type, const std::string featureName)> onPostponed;
        std::function<bool(const std::string type, const std::string featureName)> onTrigger;
        std::function<bool(const std::string type, const std::string featureName)> onUpgrade;
        std::function<bool(const std::string type, const std::string featureName)> onIgnore;
    };
    AppManagerAdapter();

    /**
     * @brief addFeature This is the function to add a feature under control of AppManagerAdapter
     * @param featureHandler
     */
    void addFeature(const std::shared_ptr<FeatureHandler> featureHandler);

    /**
     * @brief queryActionForFeature This is the function to request Query Action for feature. The
     *        result of Query Action will be called on call back function of specific feature.
     * @param name name of feature need to query action.
     */
    void queryActionForFeature(const std::string name, const std::string type = "");

    /**
     * @brief requestActiveFeature This is the function to request active specific feature.
     *        This function must be called after the Feature had been granted the permision to run
     *        by queryActionForFeature function.
     * @param name : name of feature which will be activated.
     * @return
     */
    bool requestActiveFeature(const std::string name);

    /**
     * @brief requestDeactiveFeature This is the function to request deactive specific feature.
     * @param featureName
     * @return
     */
    bool requestDeactiveFeature(const std::string name);


    /**
     * @brief getFeatureState This is the function to get the state of specific feature.
     * @param featureName
     * @return A boolen value indicate whether those feature had been activated or not.
     */
    bool getFeatureState(const std::string featureName);

    /**
     * @brief doHandleAppMgrResponse This is the function to handle the response of AppMgr from queryAction
     * @param msg
     */
    void doHandleAppMgrResponse(const sp<sl::Message>& msg);

    /**
     * @brief doHandlePostFeatureStatusChanged This is the function to handle the information about
     *        the specific feature was changed it's status.
     * @param msg
     */
    void doHandlePostFeatureStatusChanged(const sp<sl::Message>& msg);

    /**
     * @brief doHandlePostActionDelivered This is the function to handle the request to perfom some action
     *        from AppMgr. Ex: Stop or Suspend specific function in oder to other feature to run.
     * @param msg
     */
    void doHandlePostActionDelivered(const sp<sl::Message>& msg);

    void sendPostACNUS_ON_OF(int onOff);
    static bool isAvailableInRegion(const std::string feature);
private:
    /**
     * @brief This is the type of structure that contains the private
     *        properties of the instance.  It is defined in the implementation
     *        and declared here to ensure that it is scoped inside the class.
     */
    struct Impl;

    /**
     * @brief This contain private data of the instance
     *
     */
    std::shared_ptr<Impl> impl_;
};

} // namespace OEMCall App
#endif
