#pragma once

#include <memory>

namespace chat::messages
{
    class Ping;
    class Request;
    class Response;
}

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
    std::unique_ptr<chat::messages::Response> handle(const chat::messages::Request& request);

private:
    /**
     * @brief Handle a ping request.
     *
     * @param request The request to handle.
     *
     * @return A response to the request.
     */
    std::unique_ptr<chat::messages::Response> handlePing(const chat::messages::Ping& request);
};

}
