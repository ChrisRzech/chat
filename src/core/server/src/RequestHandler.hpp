#pragma once

#include "chat/messages/Request.hpp"
#include "chat/messages/Response.hpp"

#include "chat/messages/request/Ping.hpp"

#include <memory>


namespace chat::server
{

/**
 * @brief Handler for all requests.
 */
class RequestHandler
{
public:
    /**
     * @brief Construct a request handler.
     */
    RequestHandler() = default;

    /**
     * @brief Copy operations are disabled.
     * @{
     */
    RequestHandler(const RequestHandler& other) = delete;
    RequestHandler& operator=(const RequestHandler& other) = delete;
    /** @} */

    /**
     * @brief Move operations are disabled.
     * @{
     */
    RequestHandler(RequestHandler&& other) = delete;
    RequestHandler& operator=(RequestHandler&& other) = delete;
    /** @} */

    /**
     * @brief Destroy the request handler.
     */
    ~RequestHandler() = default;

    /**
     * @brief Handle a request.
     *
     * @param request The request to handle.
     *
     * @return A response to the request.
     */
    std::unique_ptr<messages::Response> handle(const messages::Request& request);

private:
    /**
     * @brief Handle a ping request.
     *
     * @param request The request to handle.
     *
     * @return A response to the request.
     */
    std::unique_ptr<messages::Response> handlePing(const messages::Ping& request);
};

}
