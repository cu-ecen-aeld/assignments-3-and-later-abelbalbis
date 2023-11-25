#! /bin/sh

do_status(){
    start-stop-daemon --status -n aesdsocket && st=$? || st=$?
    case "$st" in
        0)
            echo "aesdsocket is running ($st)."
            ;;
        3)
            echo "aesdsocket is not running ($st)."
            ;;
        *)
            echo "Unkown status of aesdsocket ($st)."
    esac
}

case "$1" in
    status)
    do_status
    ;;
    start)
    echo "Starting aesdsocket server"
    start-stop-daemon --start -n aesdsocket --exec /usr/bin/aesdsocket -- -d
    ;;
    stop)
    echo "Stoping aesdsocket server"
    start-stop-daemon --stop -n aesdsocket
    ;;
    *)
    echo "Usage: $0 {status|start|stop}"
    exit 1
esac

exit 0